// icusegments.js
// Copyright (C) 2012 IBM Corporation and Others. All Rights Reserved.

var isBase = "./icusegments";

var ajaxTimeout = 120000; // 2 minutes

var gSettings  = { dLocale: 'en_US',
					bLocale: 'en_US',
					type: -1 };
var gGlobals = {
	brks: {},
	types: {}
};

// COMMON

// ?!!!!
if(!Object.keys) {
	Object.keys = function(x) {
		var r = [];
		for (j in x) {
			r.push(j);
		}
		return r;
	};
}


function removeAllChildNodes(td) {
	if(td==null) return;
	while(td.firstChild) {
		td.removeChild(td.firstChild);
	}
}

function listenFor(what, event, fn, ievent) {
	if(!(what._stlisteners)) {
		what._stlisteners={};
	}
	
	if(what.addEventListener) {
		if(what._stlisteners[event]) {
			if(what.removeEventListener) {
				what.removeEventListener(event,what._stlisteners[event],false);
			} else {
				console.log("Err: no removeEventListener on " + what);
			}
		}
		what.addEventListener(event,fn,false);
	} else {
		if(!ievent) {
			ievent = "on"+event;
		}
		if(what._stlisteners[event]) {
			what.detachEvent(ievent,what._stlisteners[event]);
		}
		what.attachEvent(ievent,fn);
	}
	what._stlisteners[event]=fn;
}

// end common

function getJsonPost(stuff) {
	var toPost = {
		param: stuff,
		settings: gSettings
	};
	
	return JSON.stringify(toPost);
}

var initChangeBox = false;

function handleRequestUpdate() {
	// TODO: queues/mutex/timeout!
	console.log("Updating.. " + JSON.stringify(gSettings));
	try {
		dojo.byId("status").innerHTML="Updating…";
		var theStr = dojo.byId("inText").value;
		dojo.byId("output").className = "change";
		var myPostData = { str: theStr };
		dojo.xhrPost({
			url: isBase+"/break",
			handleAs:"json",
			timeout:ajaxTimeout,
			load: function(json) {
				var out = dojo.byId("output");
				
				try {
					// TODO: store into out
					if(json.err) { 
						dojo.byId("status").innerHTML="Breaks load returned err. ";
						if(json.err.message) { 
							dojo.byId("status").innerHTML="Breaks load returned err: " + json.err.message;
						}
					} else {
						dojo.byId("status").innerHTML="Showing breaks…";
						
						
						var fragment = document.createDocumentFragment();
						
						var last = 0;
						
						for(i in json.breaks.idx) {
							k = json.breaks.idx[i];
							//console.log('Break @ ' + k + " from " + last );
							var newSpan = document.createElement("span");
							newSpan.className = 'brk';
							newSpan.title = '@'+k;
							// break is after [last…k]
							if((k-last)>0) {
								newSpan.appendChild(document.createTextNode(theStr.substring(last,k)));
							}
							fragment.appendChild(newSpan);
							last = k;
						}
						if(last < theStr.length) { // chunk after last break
							fragment.appendChild(document.createTextNode(theStr.substring(last,theStr.length)));
						}
						
						removeAllChildNodes(out);
						out.className = '';
						out.appendChild(fragment);
						
						if(!initChangeBox) {
							initChangeBox=true;
							listenFor(dojo.byId("inText"),"blur", handleRequestUpdate);
							listenFor(dojo.byId("inText"),"focus", function() {
								dojo.byId("output").className="changing";
								dojo.byId("status").innerHTML="Changing text (exit field to reload).";
							});
							dojo.byId("status").innerHTML="Breaks loaded.";
						} else {
							dojo.byId("status").innerHTML="Breaks reloaded.";
						}
					}
				} catch(e) {
					console.log("JSON: " + json.toString());
					console.log("Error processing response: " + e.message);
					dojo.byId("status").innerHTML="err processing returned breaks! " + e.message;
				}
			},
			err: function(err) {
				dojo.byId("status").innerHTML="err loading breaks!";
			},
			postData:  getJsonPost(myPostData)
		});
	} catch (e) {
		console.log("Error in ajax get ", e.message);
		dojo.byId("status").innerHTML="err loading breaks! " + e.message;
	}
}

function addChooseItem(menu) {
	var e = document.createElement('option');
	e.value = null;
	e.appendChild(document.createTextNode('Choose…'));
	e.disabled = 'disabled';
	menu.appendChild(e);	
}

function setMenuFrom(menu, brks, fn) {
	var items = {};
    
    for(k in brks) {
    //    items[brks[k]] = k
    //}
    //
	//for(qq in Object.keys(items).sort()) {
    //    k = items[qq];
		var e = document.createElement('option');
		e.value = k;
		e.appendChild(document.createTextNode(brks[k]));
		menu.appendChild(e);	
		if(fn) {
			fn(e,k);
		}
	}
	return items;
}

function handleLocale(loc) {
	gSettings.bLocale = loc;
	handleRequestUpdate();
}
function setLocaleMenu(brks) {
	var menu = dojo.byId('localeList');
	removeAllChildNodes(menu);
	var results = setMenuFrom(menu, brks );
	listenFor(menu,"change", function() { handleLocale(menu.value); });
	menu.value = gSettings.bLocale;
	
	gSettings.bLocale = menu.value;
}
function handleType(type) {
	gSettings.type = type;
	handleRequestUpdate();
}
function setTypeMenu(brks) {
	var menu = dojo.byId('typeList');
	addChooseItem(menu);
	removeAllChildNodes(menu);
	gSettings.type = 0;
	
	var results = setMenuFrom(menu, brks,
		 function(e,k) { if(brks[k]=='SENTENCE') gSettings.type = k; }); 
	menu.value = gSettings.type;
	listenFor(menu,"change", function() { handleType(menu.value); });
}

dojo.ready(function() {
	dojo.byId("status").innerHTML="Fetching version:";
	
	var myPostData = {  };
	try {
		dojo.xhrPost({
			url: isBase+"/version",
			handleAs:"json",
			timeout:ajaxTimeout,
			load: function(json) {
				try {
				
            dojo.byId("icu_version").innerHTML="ICU Version: " + json.icu.version;
            if(json.icu.ulistatus && json.icu.ulistatus != 'U_ZERO_ERROR') {
                 dojo.byId("icu_version").innerHTML="ICU Version: " + json.icu.version + " - ULI breaks loaded with: " + json.icu.ulistatus;
            }
					
					gGlobals.brks = json.brks;
					setLocaleMenu(json.brks);
					
					gGlobals.types = json.types;
					setTypeMenu(json.types);
					dojo.byId("status").innerHTML="Fetched.";
					
					handleRequestUpdate();
				} catch (e) {
					dojo.byId("status").innerHTML="Error fetching version: " + e.message;
					console.log("Error: " + e.message);
				}
			},
			err: function(err) {
				dojo.byId("status").innerHTML="err!";
			},
			postData:  getJsonPost(myPostData)
		});
	} catch(e) {
		dojo.byId("status").innerHTML="Error requesting version: " + e.message;
		console.log("Error requesting version: " + e.message);
	}
});