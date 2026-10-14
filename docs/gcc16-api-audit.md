# GCC 16.2 API audit：pointer 關係、Memory SSA 與路徑爆炸

本文件以 `/tmp/gcc-16.2.0` 的官方 GCC 16.2.0 source tree 為準，並以 GCC
10.5.0 source／本機 GCC 10 plugin headers 做相容性比較。審查對象是本工作樹的
refactor 版 plugin，尤其是 `Compiler/src/plugin.cc`、`gimple_flow.cc`、
`gimple_value.cc`、`alias_oracle.cc`、`memory_value.cc`、`feasibility.cc` 與
`lifetime_engine.cc`。

## 結論先講

兩個 variable 的「指向關係」可以比單純 points-to intersection 判得更準，但必須先
區分問題；沒有一個 GCC API 能對任意兩個 pointer variable 回答完整、有方向且永遠
精確的關係。

| 想問的問題 | 可用證據 | 能安全下的結論 |
| --- | --- | --- |
| `p_1` 的值是否流到 `q_2` | `SSA_NAME_DEF_STMT`、immediate-use、typed GIMPLE accessor | 對 copy／保值 cast 可證明有向 `p_1 -> q_2`；這是 value flow，不是 alias query |
| `q_2 = PHI <p_1, r_3>` 是否代表 `p_1 -> q_2` | PHI argument 加 `gimple_phi_arg_edge` | 只在對應 incoming edge 上成立；忽略 edge 就只能說 may-flow |
| `*p` 與 `*q` 是否可能重疊 | `ptr_derefs_may_alias_p` | `false` 是此 pass 點的 no-alias 證明；`true` 只表示 may／無法排除，沒有方向 |
| `p` 與 `q` 是否必為同一 pointer value | 完整 SSA provenance、路徑條件、offset／epoch | 只有所有來源都收斂到同一保值定義時才能報 must；相同 points-to set 本身不夠 |
| `q = *slot` 的值是否來自較早的 `*slot = p` | Memory SSA VUSE/VDEF、alias filter、must-kill、路徑可行性 | 只有 unique reaching killing store，或所有可達 store 都給同一 provenance，才可報 `p -> q` |
| 任意 C 程式、任意路徑上的精確關係 | 完整語意與 reachability | 一般情況不可判定；sound 工具必須允許 `unknown`，並以資源上限造成的截斷為 `unknown` |

因此建議維持 `same-value / must-alias / may-alias / no-alias / unknown` 證據格，
另外單獨記錄有方向的 `value-flow` 與 `memory-flow` edge。不要把 `may-alias` 當成
`p points to q`，也不要用一個 boolean 同時承載這三種不同語意。

## API 邊界

GCC 沒有承諾跨 major version 的 plugin C++ ABI。即使 header 安裝在
`$(gcc -print-file-name=plugin)/include`，也不代表它是穩定 public API。

| 等級 | 本文件中的含義 | 例子 | 使用原則 |
| --- | --- | --- | --- |
| D：documented plugin surface | GCC internals manual 明確描述的 plugin 入口與 callback 機制 | `plugin_init`、`register_callback`、`PLUGIN_PASS_MANAGER_SETUP`、`register_pass_info`、`plugin_default_version_check` | 可以使用，但仍應對每個 GCC major/minor 重編並做 version check |
| I：installed compiler-internal header | GCC 安裝給 plugin 編譯的 middle-end 內部 header | `ssa-iterators.h`、`gimple-walk.h`、`tree-ssa-alias.h`、`dumpfile.h` | 可用於鎖定版本的 adapter；不可宣稱 ABI／source compatibility |
| E：GCC 16 analyzer extension seam | GCC 16 source 與 testsuite 明確供 plugin 註冊 analyzer 功能的 pub/sub seam | `analyzer_events::subscriber`、`on_ana_init::register_state_machine`、`register_known_function` | 是使用官方 analyzer engine 的正確入口，但屬 GCC 16 專用內部 C++ API，且需 `-fanalyzer` |
| B：engine/build-tree implementation | analyzer 自己建立 graph、state、diagnostic 的私有生命週期 | `impl_run_checkers`、`exploded_graph::get_or_create_node`、`feasible_graph`、`state_purge_map` | 只作設計參考；普通 pass 不應自行 new、呼叫或接管它們 |

B 級不一定表示 declaration header 完全沒有被安裝；GCC 16 的 wildcard 甚至會安裝
許多 analyzer declarations。這裡的「build-tree」是指可用的 implementation、初始化
順序、global state 與 ownership 只由 GCC 自身 source／binary 管理，沒有提供獨立的
plugin lifecycle contract。

GCC 16 的 [`gcc/Makefile.in`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/Makefile.in;hb=releases/gcc-16.2.0)
在 `PLUGIN_HEADERS` 明列 `analyzer/*.h`，所以這些 header 通常會被安裝。GCC 10 的
[`gcc/Makefile.in`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/Makefile.in;hb=releases/gcc-10.5.0)
沒有這一項。這個差異只表示 GCC 16 開始提供可編譯的 analyzer extension seam；
它不會把 `engine.cc` 的內部 orchestration 變成穩定 plugin API。

官方文件入口：

- [GCC 16.2 Plugins](https://gcc.gnu.org/onlinedocs/gcc-16.2.0/gccint/Plugins.html)
- [Plugin API](https://gcc.gnu.org/onlinedocs/gcc-16.2.0/gccint/Plugin-API.html)
- [Plugins affecting compiler passes](https://gcc.gnu.org/onlinedocs/gcc-16.2.0/gccint/Plugins-pass.html)
- [Developer options／dump options](https://gcc.gnu.org/onlinedocs/gcc-16.2.0/gcc/Developer-Options.html)

## 1. Pass registration

### 官方定義與現況

- Header：[`gcc/tree-pass.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/tree-pass.h;hb=releases/gcc-16.2.0)
- Registration implementation：[`gcc/passes.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/passes.cc;hb=releases/gcc-16.2.0)
- Pipeline：[`gcc/passes.def`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/passes.def;hb=releases/gcc-16.2.0)
- Instance generator：[`gcc/gen-pass-instances.awk`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/gen-pass-instances.awk;hb=releases/gcc-16.2.0)
- PTA pass：[`gcc/tree-ssa-structalias.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/tree-ssa-structalias.cc;hb=releases/gcc-16.2.0)

本 plugin 正確使用 `pass_data`、`simple_ipa_opt_pass`、`clone()`、
`register_pass_info`、`PLUGIN_PASS_MANAGER_SETUP` 與
`plugin_default_version_check`。`properties_required = PROP_ssa` 只保證 SSA 形式；
它不保證 IPA points-to 已成功計算。

目前 registration 是：

```text
type                         SIMPLE_IPA_PASS
reference_pass_name          "simdclone"
ref_pass_instance_number     1
position                     PASS_POS_INSERT_BEFORE
```

GCC 16.2 的 `passes.def` 有兩個 `pass_ipa_pta`：第 1 個是
`pass_ipa_oacc` 的 sub-pass，第 2 個在 `all_late_ipa_passes`。這不只是名稱搜尋的
推測，而是下列三段官方 source 共同決定的行為：

```text
tree-pass.h::register_pass_info
  ref_pass_instance_number: "Do it for every instance if it is 0."

passes.cc::position_pass
  match if ref_pass_instance_number == 0
  if 0: new_pass = pass->clone(); add_pass_instance(...)

passes.def
  pass_ipa_oacc/sub: pass_ipa_pta       # instance 1
  all_late_ipa_passes: pass_ipa_pta    # instance 2

gen-pass-instances.awk passes.def
  NEXT_PASS (pass_ipa_pta, 1)
  NEXT_PASS (pass_ipa_pta, 2)
```

`passes.cc::pass_manager::register_pass` 在 instance `0` 時還會繼續搜尋全部五個 pass
list，`position_pass` 也會遞迴搜尋 sub-pass。因此把 plugin 掛到所有 `pta` 會真的插入
兩處；OpenACC active 時，內層 PTA 可能先觸發 whole-TU 分析，不能只靠 first-run guard
補救。現行實作改用 `passes.def` 中緊跟 late IPA PTA 的唯一 simple-IPA
`pass_omp_simd_clone`（pass name `simdclone`）作 boundary，並插在它之前。GCC 10 與
GCC 16 的 late IPA 清單都有同一相鄰關係。

第 1 個 PTA 的 container `pass_ipa_oacc::gate`（官方
[`gcc/tree-ssa-loop.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/tree-ssa-loop.cc;hb=releases/gcc-16.2.0)）
要求 `optimize && flag_openacc && !seen_error()`；它是 OpenACC 專用子流程，不適合作
一般 whole-TU analyzer 的唯一掛點。第 2 個是 regular IPA passes 執行完成後的 late
IPA PTA，才是本 plugin 這種 whole-TU simple IPA consumer 在 GCC 16.2 應使用的
instance。

此外，`pass_ipa_pta::gate` 要求：

```text
optimize && flag_ipa_pta && !seen_error()
```

「排在 `pta` 後面」不代表前一個 pass 的 gate 為 true。特別是 `-O0` 或未啟用
`-fipa-pta` 時，plugin 仍可能被排程，而 `SSA_NAME_PTR_INFO` 沒有 IPA 精度。

### 現行決策

1. 使用 `simdclone` instance `1` 前方作 late-IPA anchor，並保留
   translation-unit one-shot guard 作 vendor-pipeline 防線。`1` 有
   `TODO_mark_first_instance` 的公開特例，且 upstream GCC 10/16 只有這一個
   `simdclone`。不要把 reference 改成 `pta` instance `2`：plugin registration
   發生時，既有 pass 的 `static_pass_number` 已改為全域 dump pass id；實測指定 `2`
   會以 `pass 'pta' not found` 終止，而不是選中 dump 顯示的 `pta2`。
2. gate 或執行前檢查優化／PTA 可用性；若 points-to 未提供，結果降為 `unknown`，
   不要把缺資料當 no-alias。需要時可在切入各 function 後檢查其 IPA PTA 狀態，
   但 `gimple_df` 欄位本身也屬 I 級內部 layout。
3. 若主要需求是 function-local Memory SSA，可另做 `gimple_opt_pass` 並排在
   `build_alias` 後；若必須消費 whole-unit IPA PTA，則保留 late simple IPA pass。
   兩者的 pass type 必須和 reference pass type 相符。
4. 每個 GCC 版本各自編譯 `.so`。現行 `plugin_default_version_check` 應保留；不要讓
   GCC 10 build 的 plugin binary 載入 GCC 16。

### GCC 10 → 16 風險

`pass_data`、`opt_pass::execute(function *)` 與 `register_pass_info` 的相關形狀仍相近，
但 property／TODO bits、pass 清單、同名 pass instance 數與 dump id 都有變動。
GCC 16 新增多個 property，並重排／移除部分 TODO。此 plugin 目前只使用
`PROP_ssa` 與零 TODO，source porting 風險低；`simdclone` 相鄰 late-PTA boundary
仍需在每個新增 GCC major 的 `passes.def` audit 中重新驗證。

## 2. SSA immediate uses

### 官方 API

- Header／inline implementation：[`gcc/ssa-iterators.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/ssa-iterators.h;hb=releases/gcc-16.2.0)
- 主要 API：`FOR_EACH_IMM_USE_STMT`、`FOR_EACH_IMM_USE_ON_STMT`、
  `FOR_EACH_IMM_USE_FAST`、`USE_FROM_PTR`、`gather_imm_use_stmts`

`gimple_flow.cc` 以 outer statement iterator 加 inner occurrence iterator 走訪，沒有在
迭代中修改 statement，也沒有提早 `break`／`return`；這個寫法在 GCC 16.2 是正確
的。它能建立 scalar SSA 的 copy、cast、PHI、conditional 與 pointer-offset flow。

但是 immediate-use list 只回答「這個 SSA value 在哪裡被用」，不回答哪個 store
供應某個 load，也不自動帶出可行 CFG edge。精度提升應做以下區分：

- copy 與確定保值 cast：可保留 same-value provenance。
- `POINTER_PLUS_EXPR`：可保留 allocation provenance，但不能宣稱 pointer value 相等；
  offset 及存取範圍要另記。
- PHI：逐一記錄 `gimple_phi_arg_def(phi, i)` 與
  `gimple_phi_arg_edge(phi, i)`，不要把所有 argument 無條件合成 must-flow。
- 需要修改 use／statement 時，GCC 16 可先用 `gather_imm_use_stmts(tree)` 取得去重
  vector，再修改並 `update_stmt`；唯讀掃描則保持現況即可。

### GCC 10 → 16 風險

GCC 10 的 normal immediate-use traversal 插入 marker；若提早離開，必須使用
`BREAK_FROM_IMM_USE_STMT`／`RETURN_FROM_IMM_USE_STMT`。GCC 16 改成
`auto_end_imm_use_stmt_traverse` RAII，舊兩個 macro 已刪除，並新增
`gather_imm_use_stmts`。現有完整走完 loop 的 code 同時可移植；未來不要新增依賴
GCC 10 early-exit macro 的共用 source。

## 3. GIMPLE walk

### 官方 API

- Header：[`gcc/gimple-walk.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/gimple-walk.h;hb=releases/gcc-16.2.0)
- Implementation：[`gcc/gimple-walk.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/gimple-walk.cc;hb=releases/gcc-16.2.0)
- API：`walk_gimple_seq[_mod]`、`walk_gimple_stmt`、`walk_gimple_op`、
  `walk_stmt_load_store_addr_ops`、`walk_stmt_load_store_ops`

`gimple-walk.h` 明確警告 generic `walk_gimple_*` convenience traversal 主要用在轉成
SSA 之前；SSA pass 應以 SSA operand interface 與 typed GIMPLE accessors 為主。本
plugin 目前沒有呼叫 generic walker，這是正確方向。

若要完整找出 statement 內的 memory operand，可使用較窄的
`walk_stmt_load_store_addr_ops`。其 callback 型別是：

```cpp
bool callback(gimple *stmt, tree base, tree full_operand, void *data);
```

官方 implementation 會分開回呼 load、store、address-taken；GCC 自己的 SSA／IPA
passes 及 analyzer 也使用它。它適合「枚舉 memory reference」，但不建立 reaching
definition、不套用 path condition，也不證明 pointer value direction。枚舉之後仍要
建立 `ao_ref` 並走 Memory SSA。

GCC 10 與 16 的上述 callback signature 沒有實質改變，source 風險低；其 `.cc`
implementation 與 GIMPLE kind 涵蓋範圍仍是內部細節，需以目標版本測試 call、asm、
aggregate 與 volatile case。

## 4. Points-to 與 alias oracle

### 官方 API 與真正語意

- Header：[`gcc/tree-ssa-alias.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/tree-ssa-alias.h;hb=releases/gcc-16.2.0)
- Implementation：[`gcc/tree-ssa-alias.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/tree-ssa-alias.cc;hb=releases/gcc-16.2.0)
- Points-to construction：[`gcc/tree-ssa-structalias.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/tree-ssa-structalias.cc;hb=releases/gcc-16.2.0)

重要 API：

- `SSA_NAME_PTR_INFO(name)`：取得 SSA pointer 的 `ptr_info_def`，可能為 null。
- `ptr_derefs_may_alias_p(p, q)`：剝除 no-op conversion／pointer offset，處理部分
  `ADDR_EXPR`，最後對兩個 SSA name 的 `pt_solution` 做 intersection。
- `pt_solutions_intersect(a, b)`：abstract may-set 是否相交。
- `pt_solution_singleton_or_null_p(solution, &uid)`：要求 points-to bitmap 只有一個
  var，但 solution 可能另含 null；只能作加強證據。
- `ao_ref_init`、`refs_may_alias_p[_1]`：比較完整 memory reference，包含 base、
  offset、extent，並可選 TBAA。
- `stmt_may_clobber_ref_p[_1]`：statement 是否可能 clobber reference。
- `stmt_kills_ref_p`：較強的完整覆寫／must-kill query。

`ptr_derefs_may_alias_p` 是對稱的 dereference-overlap query。它的 `true` 包含「資訊
不足所以無法排除」；只有 `false` 可以當 no-alias 證據。它不是以下任何一項：

- `p` 的值流到 `q`；
- `p` 指向 variable `q`（後者應查 `p == &q`／object identity）；
- `p` 與 `q` 在所有 execution 都相等；
- 兩者指向同一 allocation epoch。

即使兩個 solution 都只含同一個 abstract object，也可能有 null、不同 interior
offset、不同路徑或不同 lifetime epoch。因此 singleton intersection 不應直接升級為
must-alias。

本 plugin 的 `alias_oracle.cc` 已把 GCC `true` 保留為 may，並只在完整、單一且相同
provenance 時升級為 must，方向正確。仍需注意：它直接讀取 `pt_solution` bitfields；
`points_to_precise` 只是 plugin 自訂標籤，不能當 GCC 的 must 證明。raw flags 適合 dump
與保守降級，核心判斷優先使用上述官方 predicate。

### GCC 10 → 16 風險

GCC 16 的 `pt_solution` 新增 `const_pool` bit，raw layout 已改變；多個 function 也新增
參數，例如 `ptr_deref_may_alias_global_p`、`ref_may_alias_global_p`、call/global
clobber queries。`get_continuation_for_phi` 改收 `gphi *` 並增加 edge callback，
`walk_non_aliased_vuses` 也增加 edge callback。`ao_ref` helpers 亦有擴充。

現行使用的 `ptr_derefs_may_alias_p`、`dump_points_to_solution` 與
`walk_aliased_vdefs` 核心 signature 在 10→16 大致維持，但 raw struct layout 與 linked
C++ symbols 不可跨 binary 使用。建議以版本 adapter 包住所有 `tree-ssa-alias.h` 依賴。

## 5. Memory SSA：store → load 才是真正的 memory direction

GCC 的 virtual SSA 是 whole-memory version chain；`gimple_vuse(stmt) != NULL_TREE` 或
`gimple_vdef(stmt) != NULL_TREE` 只表示 statement 使用／定義 memory state，不能單獨
證明哪一個 store 把 pointer 傳給哪一個 load。

### 官方 API

- Virtual operands：`gimple_vuse`、`gimple_vdef`、`SSA_NAME_DEF_STMT`
- Alias/reference API：[`gcc/tree-ssa-alias.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/tree-ssa-alias.h;hb=releases/gcc-16.2.0)
- Walker implementation：[`gcc/tree-ssa-alias.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/tree-ssa-alias.cc;hb=releases/gcc-16.2.0)
- Main API：`walk_aliased_vdefs(ao_ref *, tree vuse, callback, data,
  bitmap *, bool *function_entry_reached, unsigned limit)`

官方 `walk_aliased_vdefs` 在 virtual PHI 分叉，並只對 `stmt_may_clobber_ref_p_1` 為真
的 definition 呼叫 callback。callback 收到 virtual SSA name；再以
`SSA_NAME_DEF_STMT(vdef)` 取得 candidate clobber。回傳 `true` 會停止該 walk；回傳值
`-1` 表示撞到 limit 而中止，必須視為 incomplete／unknown。

一個 sound、bounded 的 pointer-load resolver 應依序：

1. 用 typed accessor 或 `walk_stmt_load_store_addr_ops` 確認是 pointer load，取得完整
   load reference。
2. `ao_ref_init(&ref, load_ref)`，從 `gimple_vuse(load_stmt)` 開始。
3. `walk_aliased_vdefs` 蒐集所有可能 clobber；遇 virtual PHI 必須保留多來源。
4. 對 candidate store 檢查 RHS pointer provenance。需要 unique source 時，只有
   `stmt_kills_ref_p(store, &ref)` 或等價的完整覆寫證明才可截斷較舊 definitions。
5. call、asm、aggregate／partial store、volatile、function entry、cycle、multiple
   definitions 或任何 depth/state/step 截斷，一律保持 ambiguous／unknown。
6. 再以 CFG／path feasibility 排除不可行的 reaching definitions；若所有可行來源的
   provenance 相同，才可升級為 must memory-flow。

目前 `memory_value.cc` 已採用這個骨架：有獨立 depth/state audit、
`walk_aliased_vdefs` limit、`function_entry_reached`、`stmt_kills_ref_p`，也把 call／asm／
imprecise store 分類；它已列入 `Compiler/Makefile`，並由 `gimple_value.cc` 在 backward
provenance 遇 pointer load 時接回 stored value，也由 `lifetime_engine.cc` 建立
`memory_load` forward bridge。這比僅看 VUSE/VDEF 精確許多。保留其最重要的不變條件：
任何 walker `-1`、entry、cycle 或不完整 reference 都不得回傳 `complete`。

`walk_non_aliased_vuses` 可沿「未 clobber 此 ref」的 state 做 lookup／translation，
但其 callback contract 更複雜且 10→16 已改 signature；目前用 bounded
`walk_aliased_vdefs` 較容易維持 soundness。

### Allocator/deallocator attribute 的實測邊界

GCC 16 支援 `__attribute__((malloc(DEALLOCATOR, PTR_INDEX)))`，但 plugin 不能只讀
`DECL_IS_MALLOC` 就假定完整 ownership contract。以 late-IPA pass 直接探測

```c
extern void *project_alloc(size_t) __attribute__((malloc(free, 1)));
```

時，本機 GCC 16.2 的 declaration 顯示 `DECL_IS_MALLOC == 0`，而真正的 contract
仍保留在 `DECL_ATTRIBUTES` 的 `malloc` attribute arguments：第一個 argument 是
指向正常 `BUILT_IN_FREE` 的 `FUNCTION_DECL`，第二個是整數 `1`。因此現行
`ownership_call.cc` 直接以 `lookup_attribute("malloc", ...)` 解析 arguments；
proof-only 還要求 deallocator 的 builtin identity，不能只因某個 user function 也叫
`free` 就接受。

GCC 10 只接受無 argument 的 `malloc` attribute，會拒絕上述二參數形式。無 argument
版本只證明 fresh/no-alias return，並不證明 caller 有用 `free` 解除的義務（arena 或
GC allocator 也可能有該標記），所以 proof-only 將它保留為
`allocator-release-contract-unproven`，不產生 SA001。GCC 10 的 local wrapper 則可由
其可見 function body summary 證明 `malloc` return，不需要降低 attribute 規則。

## 6. Dump API

- Header：[`gcc/dumpfile.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/dumpfile.h;hb=releases/gcc-16.2.0)
- Implementation：[`gcc/dumpfile.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/dumpfile.cc;hb=releases/gcc-16.2.0)
- 使用中的 API：`dump_file`、`dump_flags`、`dump_printf`、
  `dump_generic_expr`、`dump_gimple_stmt`、`dump_points_to_solution`

現行做法在 pass dump active 時是正確的：先檢查 `dump_file != nullptr`，詳細證據再
檢查 `dump_flags & TDF_DETAILS`。GCC 16 `dumpfile.cc` 的 `dump_options[]` 明定
`details` 同時開啟 `TDF_DETAILS` 與三種 `MSG_*`（包含 `MSG_NOTE`），所以現行所有
`dump_printf(MSG_NOTE, ...)` 可由 `-fdump-ipa-all-details` 輸出。

同一個 source 的 `dump_manager::register_pass` 也明說 plugin pass 在 command-line
options parse 完之後才註冊，因此新 pass 不能靠單一
`-fdump-ipa-PASSNAME` switch 開啟；它會把已儲存的 `TDI_ipa_all` flags 複製給動態
pass。故對本 plugin，`-fdump-ipa-all-details` 不只是「較方便」，而是正確且可靠的
入口。plugin 不應自行配對 `dump_begin`／`dump_end` 或關閉 `dump_file`。

GCC 10 → 16 間 `dump_flag` 改為以 `uint32_t` 為 underlying type，`TDF_ERROR` 等 bit
位置改變並加入新 flags；主要 `dump_*` 呼叫形狀大致保留。不要儲存數值 bitmask、
不要假定 dump id／檔名序號，也不要跨版本共用 binary。

## 7. GCC 16 官方 analyzer 如何控制 path explosion

### Engine 的真實結構

- Entry declaration：[`gcc/analyzer/engine.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/engine.h;hb=releases/gcc-16.2.0)
- Orchestration／graph implementation：[`gcc/analyzer/engine.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/engine.cc;hb=releases/gcc-16.2.0)
- Graph declarations：[`gcc/analyzer/exploded-graph.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/exploded-graph.h;hb=releases/gcc-16.2.0)
- Program state：[`gcc/analyzer/program-state.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/program-state.h;hb=releases/gcc-16.2.0)、[`program-state.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/program-state.cc;hb=releases/gcc-16.2.0)
- State liveness purge：[`gcc/analyzer/state-purge.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/state-purge.h;hb=releases/gcc-16.2.0)、[`state-purge.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/state-purge.cc;hb=releases/gcc-16.2.0)
- Symbol complexity：[`gcc/analyzer/complexity.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/complexity.h;hb=releases/gcc-16.2.0)、[`complexity.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/complexity.cc;hb=releases/gcc-16.2.0)
- Diagnostic feasibility search：[`gcc/analyzer/feasible-graph.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/feasible-graph.h;hb=releases/gcc-16.2.0)、[`feasible-graph.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/feasible-graph.cc;hb=releases/gcc-16.2.0)

官方 analyzer 不是「列舉所有 CFG path 後再跑 alias」。其控制方式是：

| 機制 | GCC 16 class／method | 效果 |
| --- | --- | --- |
| 狀態鍵與去重 | `point_and_state`、`m_point_and_state_to_node` | exploded node 是唯一的 `(program_point, program_state)`；完全相同狀態直接 reuse |
| 排程相近狀態 | `worklist::key_t::cmp` | 依 analysis plan、call string、supergraph SCC／node 排序，讓 join-point 狀態相鄰以利 merge |
| 先 purge 再 hash | `program_state::prune_for_point`、`state_purge_map` | 移除後續不需要的 SSA／local decl bindings，再 canonicalize，增加 cache hit；不可 purge 的 checker state 仍保留 |
| 語意 merge | `program_state::can_merge_with_p` | region model 與每個 state-machine map 都能 merge 才合併；GCC 16 同時有 pairwise 與 run/bulk merging |
| 轉移時 constraint pruning | region-model edge operations | branch condition 不可滿足時停止該 successor；merge 造成的近似之後仍需診斷重播 |
| 每點上限 | `exploded_graph::get_or_create_node` | 預設每 program point 最多 8 enodes，超過時停止該點並可報 `-Wanalyzer-too-complex` |
| 全域上限 | `exploded_graph::process_worklist` | 預設約 `supergraph_nodes * 5 + 1` enodes，超過就整體 bailout |
| recursion／constraint 上限 | analyzer params | 預設 recursion depth 2、每 state constraints 20 |
| symbolic tree 上限 | `complexity`、`region_model_manager::reject_if_too_complex` | 計算 svalue／region tree 的 node count 與 depth；預設 depth 18，過深近似成 unknown。這不是 CFG path count |
| 診斷可行性重播 | `exploded_path::feasible_p`、`feasibility_state`、`feasible_graph` | 候選 diagnostic path 逐 edge 重播 constraint；`feasible_graph` 搜尋替代路徑並記錄 rejected constraint |

GCC 16.2 的預設值來自
[`gcc/analyzer/analyzer.opt`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/analyzer.opt;hb=releases/gcc-16.2.0)：
`analyzer-bb-explosion-factor=5`、`max-enodes-per-program-point=8`、
`max-constraints=20`、`max-infeasible-edges=10`、`max-recursion-depth=2`、
`max-svalue-depth=18`、`min-snodes-for-call-summary=10`。這些是 GCC analyzer 的
policy，不應硬套成 plugin 的相容 API；可把相同概念與可設定 limits 移植到自己的
abstract interpreter。

### 與本 plugin 的差距及可採用方法

本 plugin 的 CFG solver 目前會在 basic block 層合併 ownership bit-state，另以 bounded
structural／SMT query 驗證部分 event sequence。它沒有 GCC analyzer 的 region model、
call string、per-symbol checker map 或完整 constraint manager。因此可採用的是設計：

1. 將 visited key 從單純 block 擴成必要的 product state，例如
   `(bb, event-stage, ownership-state, bounded path facts)`。
2. 在 join 做明確 lattice merge；保留 provenance 的 `complete` flag，任何不同來源或
   truncation 降級為 may／unknown。
3. 利用 SSA liveness／需求分析 purge 與目前 query 無關的 symbol facts，再 hash state。
4. 設 per-point、global、recursion、Memory-SSA-walk 與 SMT budget；撞 limit 必須記錄
   truncated，不可當 infeasible。
5. 先用便宜 CFG／dominance／edge condition pruning，最後對實際要報告的 witness 做
   feasibility replay。官方 `feasible_graph` 是 diagnostic path search，不是普通 plugin
   可以借來遍歷自身 CFG 的 library。

### 正確的 GCC 16 analyzer plugin 路徑

GCC 16 新增刻意給 subsystems／plugins 使用的 pub/sub seam：

- [`gcc/context.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/context.h;hb=releases/gcc-16.2.0)
- [`gcc/channels.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/channels.h;hb=releases/gcc-16.2.0)
- [`gcc/pub-sub.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/pub-sub.h;hb=releases/gcc-16.2.0)
- [`gcc/analyzer/common.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/common.h;hb=releases/gcc-16.2.0)
- [`gcc/analyzer/sm.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/sm.h;hb=releases/gcc-16.2.0)

Plugin 可實作 `gcc::topics::analyzer_events::subscriber`，訂閱
`g->get_channels().analyzer_events_channel`，並在 `on_ana_init` 呼叫：

```cpp
message.register_state_machine(std::unique_ptr<ana::state_machine>);
message.register_known_function(name,
                                std::unique_ptr<ana::known_function>);
```

官方 testsuite 範例是
[`analyzer_gil_plugin.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/testsuite/gcc.dg/plugin/analyzer_gil_plugin.cc;hb=releases/gcc-16.2.0)
與
[`analyzer_known_fns_plugin.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/testsuite/gcc.dg/plugin/analyzer_known_fns_plugin.cc;hb=releases/gcc-16.2.0)。
`engine.cc::impl_run_checkers` 會在內建 checker／known function 建立後 publish
`on_ana_init`，再由 engine 自己建 `extrinsic_state` 與 exploded graph。這就是想取得
官方 path-sensitive engine 時應走的界線。

此 adapter 必須：

- 以 `#if ENABLE_ANALYZER` 編譯；否則呼叫／回報 `sorry_no_analyzer`。
- 要求使用者以 `-fanalyzer` 執行，並保留精確 GCC version check。
- 對 GCC 10 使用另一條 standalone pass 路徑；GCC 10 沒有此 channel，且沒有安裝
  `analyzer/*.h`。

不要從普通 IPA/GIMPLE pass 直接呼叫 `ana::run_checkers()`，也不要自行建
`supergraph`、`state_purge_map`、`engine`、`exploded_graph` 或 `feasible_graph`。
`engine.h` 雖宣告 `run_checkers()`，真正初始化順序在 `engine.cc::impl_run_checkers`；
直接重入會與官方 analyzer pass、global flags、diagnostic manager、LTO 及 object
lifetime 耦合，沒有受支援的 ownership contract。

### GCC 10 → 16 analyzer 相容性

兩版都有 `ana::run_checkers()` 宣告與早期 exploded graph 概念，但內部 API 已大幅
改寫：GCC 10 的 state purge 只追 SSA name／`function_point`，GCC 16 還處理 local
decl、address-taken 與新的 region model；GCC 10 的 feasibility check 內嵌於
`exploded_path::feasible_p`，尚無現在的獨立 `feasible-graph.*`；`complexity.*` 與
GCC 16 bulk-merge/channel seam 也不能假定存在。這些 class 不能靠 `#if GCC_VERSION`
小修 signature 就安全共用。

GCC 10 參考 source：

- [`analyzer/engine.cc`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/engine.cc;hb=releases/gcc-10.5.0)
- [`analyzer/exploded-graph.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/exploded-graph.h;hb=releases/gcc-10.5.0)
- [`analyzer/program-state.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/program-state.h;hb=releases/gcc-10.5.0)
- [`analyzer/state-purge.h`](https://gcc.gnu.org/git/?p=gcc.git;a=blob;f=gcc/analyzer/state-purge.h;hb=releases/gcc-10.5.0)

## 優先落地順序

1. **pass readiness／multiplicity**：以 late-IPA PTA 後方唯一 `simdclone` 作 anchor，
   保留 translation-unit one-shot guard，並明確記錄 PTA unavailable；不要用 dump
   尾碼 `pta2` 當 registration selector。
2. **保留分層關係模型**：有向 scalar SSA flow、Memory SSA flow、對稱 alias relation
   分開，不再用 alias boolean 推導方向。
3. **完成 PHI edge 與 memory witness**：報告中附 incoming edge、reaching store、
   must-kill／may-clobber 與 truncation 原因。
4. **所有 budget 都 fail-open to unknown**：SSA depth、VDEF walk、CFG state、SMT、
   recursion 任一用盡都不能變成 no-alias／infeasible。
5. **需要官方 analyzer 精度時另做 GCC 16 adapter**：只透過 `on_ana_init` 註冊
   state machine／known function，讓 GCC 擁有 graph 與 path lifecycle；不要把私有
   engine 搬進現有 pass。

最終可達成的目標不是「任何兩個 variable 都判對方向」，而是對可證明 case 給出
可重播的 witness，對 may case 保守保留，並對不可判定或資源不足 case 誠實回傳
`unknown`。這會同時提高 precision 與 soundness。
