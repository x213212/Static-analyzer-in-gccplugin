#include <stdlib.h>
__attribute__((noinline))
static void maybe_release(void *value, int release) {
  if (release) free(value);
}
void generated_0(int g0, int g1, int g2, int g3, int release) {
  void *value = malloc(32);
  if (value == NULL) return;
  if (g1 == 0) {
    if (g0 != 0) {
      if (g3 == 0) {
        if (g3 != 0) {
          maybe_release(value, release);
        } else {
          free(value);
        }
      } else {
        if (g1 != 0) {
          (void)value;
        } else {
          free(value);
        }
      }
    } else {
      if (g1 == 0) {
        if (g1 != 0) {
          free(value);
        } else {
          free(value);
        }
      } else {
        if (g3 != 0) {
          free(value);
        } else {
          maybe_release(value, release);
        }
      }
    }
  } else {
    if (g0 != 0) {
      if (g0 == 0) {
        if (g1 == 0) {
          free(value);
        } else {
          maybe_release(value, release);
        }
      } else {
        if (g1 == 0) {
          free(value);
        } else {
          maybe_release(value, release);
        }
      }
    } else {
      free(value);
    }
  }
}
void generated_1(int g0, int g1, int g2, int g3, int release) {
  void *value = malloc(32);
  if (value == NULL) return;
  free(value);
}
void generated_2(int g0, int g1, int g2, int g3, int release) {
  void *value = malloc(32);
  if (value == NULL) return;
  (void)value;
}
void generated_3(int g0, int g1, int g2, int g3, int release) {
  void *value = malloc(32);
  if (value == NULL) return;
  if (g3 == 0) {
    if (g0 == 0) {
      (void)value;
    } else {
      maybe_release(value, release);
    }
  } else {
    if (g3 == 0) {
      free(value);
    } else {
      if (g1 == 0) {
        if (g0 == 0) {
          free(value);
        } else {
          maybe_release(value, release);
        }
      } else {
        if (g0 != 0) {
          free(value);
        } else {
          maybe_release(value, release);
        }
      }
    }
  }
}
void generated_4(int g0, int g1, int g2, int g3, int release) {
  void *value = malloc(32);
  if (value == NULL) return;
  if (g2 != 0) {
    if (g0 != 0) {
      if (g0 != 0) {
        free(value);
      } else {
        if (g2 != 0) {
          free(value);
        } else {
          free(value);
        }
      }
    } else {
      if (g2 != 0) {
        if (g2 == 0) {
          maybe_release(value, release);
        } else {
          (void)value;
        }
      } else {
        if (g3 != 0) {
          free(value);
        } else {
          (void)value;
        }
      }
    }
  } else {
    if (g2 == 0) {
      free(value);
    } else {
      free(value);
    }
  }
}
void generated_5(int g0, int g1, int g2, int g3, int release) {
  void *value = malloc(32);
  if (value == NULL) return;
  if (g3 != 0) {
    free(value);
  } else {
    if (g0 != 0) {
      if (g3 == 0) {
        if (g0 != 0) {
          free(value);
        } else {
          maybe_release(value, release);
        }
      } else {
        if (g3 == 0) {
          free(value);
        } else {
          maybe_release(value, release);
        }
      }
    } else {
      if (g0 != 0) {
        if (g0 != 0) {
          maybe_release(value, release);
        } else {
          free(value);
        }
      } else {
        free(value);
      }
    }
  }
}
void generated_6(int g0, int g1, int g2, int g3, int release) {
  void *value = malloc(32);
  if (value == NULL) return;
  if (g1 != 0) {
    (void)value;
  } else {
    if (g1 != 0) {
      if (g0 == 0) {
        if (g0 != 0) {
          maybe_release(value, release);
        } else {
          free(value);
        }
      } else {
        free(value);
      }
    } else {
      free(value);
    }
  }
}
void generated_7(int g0, int g1, int g2, int g3, int release) {
  void *value = malloc(32);
  if (value == NULL) return;
  if (g2 != 0) {
    free(value);
  } else {
    if (g2 != 0) {
      maybe_release(value, release);
    } else {
      maybe_release(value, release);
    }
  }
}
