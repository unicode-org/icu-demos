//--------------------------------------------------------------------
// Samples for the ICU Transform Demo.  Export two functions:
// emitSamplesGUI() -- emit the <option> tags for the samples menu
// handleInsertSamples() -- handle the selection from the samples menu

// Sample text
FOX =
    '"The quick brown -- fox."';

ACCENTS =
    "\"La mort d'Olivier B\u00E9caille\" -- \u00C9mile Zola;\n"+
    "\"Das Verm\u00E4chtnis des alten Pilgers\" von Rainer M. Schr\u00F6der (\u00D6sterreich, \u00D6VP);\n"+
    "\"Smy\u010Dcov\u00FD koncert As dur\" -- Anton\u00EDn Dvo\u0159\u00E1k.";

NAMES =
    "\uae40, \uad6d\uc0bc\n"+
    "\uc774, \uae30\uc218\n"+
    "\uae40, \uba85\ud76c\n"+
    "\uc774, \ud61c\ubbfc\n"+
    "\uac15, \uc815\ub2d8\n"+
    "\uc815, \ubcd1\ud638\n"+
    "\u305f\u3051\u3060, \u307e\u3055\u3086\u304d\n"+
    "\u307e\u3059\u3060, \u3088\u3057\u3072\u3053\n"+
    "\u3084\u307e\u3082\u3068, \u306e\u307c\u308b\n"+
    "\u304a\u304a\u306f\u3089, \u307e\u306a\u3076\n"+
    "\u3075\u306a\u3064, \u304b\u305a\u3072\u3053\n"+
    "\u305f\u3058\u308a, \u307f\u306d\u306f\u308b\n"+
    "\u3064\u3060, \u305f\u304b\u3057\n"+
    "\u306a\u304c\u304a, \u3068\u3082\u307f\n"+
    "\u304a\u304a\u3059\u304e, \u3072\u308d\u3086\u304d\n"+
    "\u041a\u043e\u0440\u043d\u0438\u043b\u044c\u0435\u0432, \u041a\u0438\u0440\u0438\u043b\u043b\n"+
    "\u0410\u0440\u0431\u0430\u0442\u043e\u0432, \u0413\u0435\u043e\u0440\u0433\u0438\u0439\n"+
    "\u041a\u043e\u0432\u0430\u043b\u0435\u0432, \u041d\u0438\u043a\u043e\u043b\u0430\u0439\n"+
    "\u0413\u043e\u0440\u0431\u0430\u0447\u0435\u0432, \u041c\u0438\u0445\u0430\u0438\u043b\n"+
    "\u0410\u0440\u0431\u0443\u0437\u043e\u0432, \u041c\u0438\u0445\u0430\u0438\u043b\n"+
    "\u041a\u043e\u0437\u044b\u0440\u0435\u0432, \u0410\u043d\u0434\u0440\u0435\u0439\n"+
    "\u0414\u0443\u0431\u0440\u043e\u0432\u0438\u043d\u0430, \u0415\u043b\u0435\u043d\u0430\n"+
    "\u041a\u0430\u0437\u0430\u043a\u043e\u0432\u0430, \u0420\u0438\u043c\u043c\u0430\n"+
    "\u0424\u0435\u0434\u043e\u0440\u043e\u0432, \u0411\u043e\u0440\u0438\u0441\n"+
    "\u041d\u0435\u043c\u0446\u043e\u0432, \u0411\u043e\u0440\u0438\u0441\n"+
    "\u0414\u043e\u0433\u0438\u043b\u0435\u0432\u0430, \u0422\u0430\u0442\u044c\u044f\u043d\u0430\n"+
    "\u039a\u03b1\u03c6\u03b5\u03c4\u03b6\u03cc\u03c0\u03bf\u03c5\u03bb\u03bf\u03c2, \u0398\u03b5\u03cc\u03c6\u03b9\u03bb\u03bf\u03c2\n"+
    "\u03a1\u03bf\u03cd\u03c4\u03c3\u03b7, \u0386\u03bd\u03bd\u03b1\n"+
    "\u039a\u03b1\u03bb\u03bf\u03cd\u03b4\u03b7\u03c2, \u03a7\u03c1\u03ae\u03c3\u03c4\u03bf\u03c2\n"+
    "\u039b\u03af\u03bd\u03c4\u03c7\u03bf\u03c1\u03c3\u03c4, \u0391\u03bd\u03c4\u03ce\u03bd\u03b9\n"+
    "\u0398\u03b5\u03bf\u03b4\u03c9\u03c1\u03ac\u03c4\u03bf\u03c5, \u0395\u03bb\u03ad\u03bd\u03b7\n"+
    "\u039d\u03b1\u03b4\u03ac\u03bb\u03b7\u03c2, \u039a\u03ce\u03c3\u03c4\u03b1\u03c2\n"+
    "\u039a\u03b9\u03b1\u03c0\u03b5\u03ba\u03ac\u03ba\u03b7, \u03a7\u03b1\u03c1\u03ac\n"+
    "\u039a\u03bf\u03bd\u03c4\u03bf\u03b3\u03b9\u03b1\u03bd\u03bd\u03ac\u03c4\u03bf\u03c2, \u0392\u03b1\u03c3\u03af\u03bb\u03b7\u03c2";

// Sample v-table (maps sample names to [<input text>, <id>])
SAMPLES = new Object();
SAMPLES["Casing"] = [FOX, "Title"];
SAMPLES["Publishing"] = [FOX, "Publishing"];
SAMPLES["Compound"] = [FOX, "Title; Publishing"];
SAMPLES["Filters"] = [FOX, "[^q] Title; Publishing"];
SAMPLES["Filters 2"] = [FOX, "[:Punctuation:] Remove"];
SAMPLES["Filters 3"] = [FOX, "[[:Punctuation:]-[.,]] Remove"];
SAMPLES["Names"] = [NAMES, "Latin; Title"];
SAMPLES["Names (Variant)"] = [NAMES, "Greek-Latin/UNGEGN; Latin; Title"];
SAMPLES["Accents"] = [ACCENTS, "Publishing"];
SAMPLES["Accents 2"] = [ACCENTS, "Upper; NFD; [\\u0301]Remove; NFC"];

// Exported API
function emitSamplesGUI() {
  for (s in SAMPLES) {
    document.writeln('<option>' + s + '</option>');
  }
}

// Exported API
function handleInsertSamples(sample) {
  var s = SAMPLES[sample];
  if (s != null) {
    document.FORM.ARG_INPUT.value = s[0];

    document.FORM.OUTPUT1.value = "(Press the Transform button to see results.)";
    document.FORM.SELECT_SOURCE1.selectedIndex = 0; // (Compound)
    document.FORM.COMPOUND_ID1.value = s[1];

    document.FORM.OUTPUT2.value = "";
    document.FORM.SELECT_SOURCE2.selectedIndex = 0; // (Compound)
    document.FORM.COMPOUND_ID2.value = "";
  }
}

SAMPLES_LOADED = 1;
