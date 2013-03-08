// Copyright (C) 2013 IBM Corporation and Others. All Rights Reserved.

var lxstrs = {
    region: "(view by Region)",
    language: "(view by Language)"
};

function lxsetup() {
    // region table
    var choose_select = document.getElementById("choose_select");
    var choose_lang = document.getElementById("choose_lang");
    var choose_region = document.getElementById("choose_region");

    var _cur = null;
    var _region = document.createTextNode(lxstrs.region);
    var _lang   = document.createTextNode(lxstrs.language);

    var toggled = false;
    var switcher = null;

    function lxupdatehash() {
        if(toggled) {
            window.location.hash = "#region";
        } else {
            window.location.hash = "#language";
        }
    }

    function lxupdateswitcher() {
        if(_cur) {
            switcher.removeChild(_cur);
            _cur = null;
        }
        if(toggled) {
            choose_lang.style.display='none';
            choose_region.style.display=null;
            _cur = _lang;
        } else {
            choose_region.style.display='none';
            choose_lang.style.display=null;
            _cur = _region;
        }
        switcher.appendChild(_cur);
    }

    if(choose_select && choose_lang && choose_region) {
        var switcher = document.createElement("button");
        if(window.location.hash && window.location.hash == '#region') {
            toggled = true;
        } else {
            toggled = false;
        }
        lxupdateswitcher();
        choose_select.appendChild(switcher);

        switcher.onclick = function() {
            toggled = !toggled;
            lxupdatehash();
            lxupdateswitcher();
            return false;
        }
    }
}

window.onload = function() {
    lxsetup();
}
