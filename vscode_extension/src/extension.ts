// The module 'vscode' contains the VS Code extensibility API
// Import the module and reference it with the alias vscode in your code below
import * as vscode from 'vscode';
import { DebugSession } from 'vscode';
var fs = require('fs');
// this method is called when your extension is activated
// your extension is activated the very first time the command is executed
var arrays = [""]; // empty array
var init = 0;
export function activate(context: vscode.ExtensionContext) {
	// Use the console to output diagnostic information (console.log) and errors (console.error)
	// This line of code will only be executed once when your extension is activated

	// let globalStoragePath = context.globalStoragePath;
	context.subscriptions.push(vscode.debug.onDidChangeBreakpoints(
		session => {
			var fs = require('fs');

			const globalStoragePath = context.globalStoragePath;
			if (session['removed'].length > 0) {
				let i = 0;
				let find = -1;
				for (let array of arrays) {
					for (let sess of session['removed']) {
						console.log(sess['location']['uri']['path']);
						// console.log("sess");

						if (array['path'] == sess['location']['uri']['path']) {
							// for (let array of arrays) {

							// }
							let linesrc = (Number(sess['location']['range']['_start']['_line']));
							if (Number(array['line']) == linesrc) {
								console.log("find");
								find = i;
								break;
							}
						}

					}

					i++;
				}
				if (find >= 0) {
					arrays.splice(find, 1);
				}
				console.log(arrays);
				fs.unlink(globalStoragePath + '/breakpoint.txt', (err: any) => {
					if (err) console.log('init error');
				})
				for (let array of arrays) {
					fs.appendFile(globalStoragePath + '/breakpoint.txt', array['path'] + " " + (Number(array['line']) + 1) + "\n", function (err: any) {
						if (err)
							console.log(err);
						else
							console.log('Write operation complete.');
					});
				}

			}

			if (session['added'].length > 0) {
				let find = -1;

				for (let array of arrays) {
					for (let sess of session['added']) {
						console.log(sess['location']['uri']['path']);
						// console.log("sess");

						if (array['path'] == sess['location']['uri']['path']) {
							// for (let array of arrays) {

							// }
							let linesrc = (Number(sess['location']['range']['_start']['_line']));
							if (Number(array['line']) == linesrc) {
								console.log("find");
								find = i;
								break;
							}
						}

					}

				}


				if (find == -1)
					for (let sess of session['added']) {

						let id = sess['_id'];
						let pathstc = sess['location']['uri']['path'];
						let linesrc = (Number(sess['location']['range']['_start']['_line']));
						arrays.push({ bid: id, path: pathstc, line: linesrc });


					}
				console.log(arrays);

				fs.unlink(globalStoragePath + '/breakpoint.txt', (err: any) => {
					if (err) console.log('init error');
				})
				for (let array of arrays) {
					fs.appendFile(globalStoragePath + '/breakpoint.txt', array['path'] + " " + (Number(array['line']) + 1) + "\n", function (err: any) {
						if (err)
							console.log(err);
						else
							console.log('Write operation complete.');
					});
				}
			}
		}
	))

	// const storagePathCast = (context.storageUri as vscode.Uri).path;
	// const globalStoragePath = context.globalStorageUri.path;
	// The command has been defined in the package.json file
	// Now provide the implementation of the command with registerCommand
	// The commandId parameter must match the command field in package.json

	const disposable = vscode.commands.registerCommand('extension.helloWorld', () => {

		// The code you place here will be executed every time your command is executed
		vscode.debug.onDidReceiveDebugSessionCustomEvent(event => {
			// console.log(event.event);
			if (event.event == 'stopped') {

				console.log("Hello World!");
			}
		})

		vscode.window.showInformationMessage('welcome use Gcc plugin analyzer');
		const storagePath = context.storagePath;
		const globalStoragePath = context.globalStoragePath;
		console.log("storagePath:" + globalStoragePath);

		vscode.window.showInformationMessage(globalStoragePath + "/breakpoint.txt")
		var fs = require('fs');
		// fs.appendFile(globalStoragePath + '/breakpoint.txt',"") , function (err: any) {
		// 	if (err)
		// 		console.log(err);
		// 	else
		// 		console.log('Write operation complete.');
		// });
		// fs.appendFile(globalStoragePath + '/breakpoint.txt', function (err: any) {
		// 	if (err) 
		// 	console.log('The “data to append” was appended to file!');
		// 	else
		// 		console.log('Write operation complete.');
		//   });
		fs.exists(globalStoragePath + '/breakpoint.txt', function (exists: any) {
			if (!exists) {
				fs.writeFile(globalStoragePath + '/breakpoint.txt',"",function (err:any) {
					if (err) 
					console.log('The “data to append” was appended to file!');
					else
						console.log('Write operation complete.');
				})
			}
		});
	

	});


	const disposable2 = vscode.commands.registerCommand('extension.removebreakp', () => {

		// The code you place here will be executed every time your command is executed
		vscode.debug.onDidReceiveDebugSessionCustomEvent(event => {
			// console.log(event.event);
			if (event.event == 'stopped') {

				console.log("Hello World!");
			}
		})

		vscode.window.showInformationMessage('remove all breakpoint');
		const storagePath = context.storagePath;
		const globalStoragePath = context.globalStoragePath;
		console.log("storagePath:" + globalStoragePath);
		arrays = [];
		var filePath = globalStoragePath + "/breakpoint.txt";
		fs.exists(filePath, function (exists: any) {
			if (exists) {
				fs.unlinkSync(filePath);
			}
		});

		vscode.window.showInformationMessage(globalStoragePath + "/breakpoint.txt")



	});
	context.subscriptions.push(disposable);
	context.subscriptions.push(disposable2);
	console.log('Congratulations, your extension "GCC Plugin load successful!" is now active!');
}
