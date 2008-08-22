// Copyright (c) 2008-2008 IBM Corporation and others. All Rights Reserved.

package Test;

import java.util.List;
import java.util.ArrayList;
import java.util.HashMap;
//import java.util.Collections;
import java.util.Iterator;

public class testLocale {

	/**
	 * @param args
	 */
	private static boolean hasCountry = false;
	private static boolean hasScript = false;

	public static List<String> resultCandidateList = new ArrayList<String>();
	public static List<String> JDKCandidateList = new ArrayList<String>();
	public static String ext = null;
	
	private static final String inputLocale[] = {
		"en",
		"en_US",
		"en_US_POSIX",
		"en_Latn",
		"en_Latn_US",
		"zh_CN",
		"zh_CN_Beijing",
		"zh_Hans_CN",
		"zh_Hant_CN",
		"zh_Hant",
		"pa",
		"pa_IN",
		"pa_PK",
		"no",
		"nb",
		"no_NO",
		"nb_NO",
		"nn_NO",
		"no_NO_NY",
		"en_US__@x=new-user",
		"ja_JP_JP",
		"ja_JP_JP_@currency=JPY",
		"ja_JP_JP_@calendar=gregorian"
	};
	
	private static List<String> inputList = new ArrayList<String>();
	
	private static final String candidateLocale[][]={
		{"en"},
		{"en_US", "en"},
		{"en_US_POSIX", "en_US", "en"},
		{"en_Latn", "en"},
		{"en_Latn_US", "en_Latn", "en_US", "en"},
		{"zh_CN", "zh_Hans_CN", "zh_Hans", "zh"},
		{"zh_CN_Beijing", "zh_Hans_CN_Beijing", "zh_Hans_CN", "zh_Hans", "zh_CN", "zh"},
		{"zh_Hans_CN","zh_Hans", "zh_CN", "zh"},
		{"zh_Hant_CN", "zh_Hant", "zh_CN", "zh"},
		{"zh_Hant", "zh_Hant_TW", "zh_TW", "zh"},
		{"pa", "pa_Guru"},
		{"pa_IN", "pa_Guru_IN", "pa_Guru", "pa"},
		{"pa_PK", "pa_Arab_PK", "pa_Arab", "pa"},
		{"no", "nb"},
		{"nb", "no"},
		{"no_NO", "nb_NO", "nb", "no"},
		{"nb_NO", "nb", "no_NO", "no"},
		{"nn_NO", "nn", "no_NO", "no"},
		{"no_NO_NY", "nn_NO_NY", "nn_NO", "nn", "no_NO", "no"},
		{"en_US", "en"},
		{"ja_JP_JP", "ja_JP", "ja"},
		{"ja_JP_JP", "ja_JP", "ja"},
		{"ja_JP_JP", "ja_JP", "ja"},
	};
	
	private static final String extensions[] = {
		null,	null,	null,	null,	null,	null,
		null,	null,	null,	null,	null,	null,	
		null,   null,  null,	null,   null,   null,
		null,
		"x=new-user",
		"calendar=japanese",
		"calendar=japanese;currency=JPY",
		"calendar=gregorian"
	};


	private static final String compatibilityKeyFormA[] = {
		"ja_JP_JP",
		"th_TH_TH"
	};


	private static final String compatibilityKeyFormB[]= {
		"_@calendar=japanese",
		"_@digit=Thai "
	};

	private static List<String> formAList = new ArrayList<String>();
	
	private static List<String> formBList = new ArrayList<String>();
	
	private static final String localeMappingFormA[] = {
		"no",
		"zh_CN",
		"zh_SG",
		"zh_HK",
		"zh_TW"
	};


	private static final String localeMappingFormB[]= {
		"nn",
		"zh_Hans_CN",
		"zh_Hans_SG",
		"zh_Hant_HK",
		"zh_Hant_TW"
	};

	private static List<String> localeFormAList = new ArrayList<String>();
	private static List<String> localeFormBList = new ArrayList<String>();
	
	private static final String languageMappingFormA[] = {
		"zh_Hans",
		"zh_Hant"
	};


	private static final String languageMappingFormB[]= {
		"zh_Hans_CN",
		"zh_Hant_TW"
	};

	private static List<String> languageMappingformAList = new ArrayList<String>();
	private static List<String> languageMappingformBList = new ArrayList<String>();
	
	private static final String scriptMappingFormA[] = {
		"pa_PK",
		"pa",
		"uz_AF",
		"uz"
	};


	private static final String scriptMappingFormB[]= {
		"pa_Arab_PK",
		"pa_Guru",
		"uz_Arab_AF",
		"uz_Cyrl"
	};

	private static List<String> scriptMappingformAList = new ArrayList<String>();
	private static List<String> scriptMappingformBList = new ArrayList<String>();
	
	private static final String macroLanguageFormA[] = {
		"no",
		"sh"
	};


	private static final String individualLanguageFormB[][]= {
		{"nb","no"},
		{"nn","no"},
		{"bs","sh"},
		{"hr","sh"},
		{"sr","sh"}
	};

	private static final String individualLanguageFormC[]= {
		"nb",
		"sr"
	};

	private static List<String> macroLanguageformAList = new ArrayList<String>();
	private static HashMap<String, String> macroLanguageformAtable = new HashMap<String,String>();
	
	private static void initialize(){
		int k =0;
		
		for(k=0;k<inputLocale.length;k++){
			inputList.add(inputLocale[k]);
		}
		
		for(k=0;k<compatibilityKeyFormA.length;k++){
			formAList.add(compatibilityKeyFormA[k]);
		}
		
		for(k=0;k<compatibilityKeyFormB.length;k++){
			formBList.add(compatibilityKeyFormB[k]);
		}
		
		for(k=0;k<localeMappingFormA.length;k++){
			localeFormAList.add(localeMappingFormA[k]);
		}
		
		for(k=0;k<localeMappingFormB.length;k++){
			localeFormBList.add(localeMappingFormB[k]);
		}
		
		for(k=0;k<languageMappingFormA.length;k++){
			languageMappingformAList.add(languageMappingFormA[k]);
		}
		
		for(k=0;k<languageMappingFormB.length;k++){
			languageMappingformBList.add(languageMappingFormB[k]);
		}
		
		for(k=0;k<scriptMappingFormA.length;k++){
			scriptMappingformAList.add(scriptMappingFormA[k]);
		}
		
		for(k=0;k<scriptMappingFormB.length;k++){
			scriptMappingformBList.add(scriptMappingFormB[k]);
		}
		
		for(k=0;k<macroLanguageFormA.length;k++){
			macroLanguageformAList.add(macroLanguageFormA[k]);
		}
		
		for(k=0;k<individualLanguageFormB.length;k++){
			macroLanguageformAtable.put(individualLanguageFormB[k][0], individualLanguageFormB[k][1]);
		} 
	
	}
	
	public static String[] getCandidateList(String Locale){
		
		initialize();
		resultCandidateList.clear();
		//Algorithm
		
		String locale = Locale;
		String baseLocaleOrg, baseLocaleTemp = null;
		String script[] = null;
		String extension = null, baseLocale;
		String localeMapping;
		int step = 10;
		int indexOfVariant = 0;
		int indexOfCountry = 0;
	//	if(inputList.contains(locale)){
			
			// Step 1
			if(locale.contains("@")){
				baseLocaleOrg = baseLocale = locale.substring(0, locale.indexOf("@")-1);
				extension = locale.substring(locale.indexOf("@")+1);
				while(baseLocale.charAt(baseLocale.length()-1)=='_'){
					baseLocaleOrg = baseLocale = baseLocale.substring(0, baseLocale.length()-1);
				}
			}else {
				baseLocaleOrg = baseLocale = locale;
				extension = null;
                                while(baseLocale.charAt(baseLocale.length()-1)=='_'){
					baseLocaleOrg = baseLocale = baseLocale.substring(0, baseLocale.length()-1);
				}
			}
			
			int numberOFUnderscore = baseLocaleOrg.length()-baseLocaleOrg.replaceAll("_", "").length();
			script = baseLocaleOrg.split("_", 4);
			//Step 2
			resultCandidateList.add(baseLocaleOrg);
						
			//Step 3
			if(formAList.contains(baseLocaleOrg)){
				String tempExtension = formBList.get(formAList.indexOf(baseLocaleOrg));
				String key = tempExtension.substring(tempExtension.indexOf("@")+1, tempExtension.indexOf("="));
				if(extension==null){
					baseLocale +=tempExtension;
					extension = tempExtension.substring(2, tempExtension.length());
				}else{
					if(!extension.contains(key)){
						//extension = tempExtension.substring(tempExtension.indexOf("@")+1)+";"+extension;
						extension = tempExtension +";"+extension;
						//baseLocale +=tempExtension+";"+extension;
						baseLocale +=extension;
						extension = extension.substring(2, extension.length());
					}
				}
			}
			
			if(script.length>1  && ((localeFormAList.contains(script[0]) && script.length>=3)||localeFormAList.contains(script[0]+"_"+script[1]))){ //Step 4 
				if(!localeFormAList.contains(localeMapping=baseLocaleOrg.substring(0, 5))){
					baseLocale = baseLocaleOrg.replaceFirst(script[0], localeFormBList.get(localeFormAList.indexOf(script[0])));
				}else{
					baseLocale = baseLocaleOrg.replaceFirst(localeMapping, localeFormBList.get(localeFormAList.indexOf(localeMapping)));
				}
				step = 9;
			} else if(script.length>=2 && languageMappingformAList.contains(script[0]+"_"+script[1])){ //Step 5
				if(script.length>=3 && script[2]==null){
					baseLocale = baseLocaleOrg.replaceFirst(baseLocaleOrg.substring(0, 8), languageMappingformBList.get(languageMappingformAList.indexOf(baseLocaleOrg.substring(0, 7))));
				}else if(script.length==2){
					baseLocale = baseLocaleOrg.replaceFirst(baseLocale.substring(0, 7), languageMappingformBList.get(languageMappingformAList.indexOf(baseLocaleOrg.substring(0, 7))));
				}
				step = 9;
			} else if((script.length>=2 && script[1].length()!=4 && scriptMappingformAList.contains(script[0])) || scriptMappingformAList.contains(script[0])){ //Step 6
				if(script.length>1 && scriptMappingformAList.contains(script[0]+"_"+script[1])){
					baseLocale = baseLocaleOrg.replaceFirst(script[0]+"_"+script[1], scriptMappingformBList.get(scriptMappingformAList.indexOf(script[0]+"_"+script[1])));
				}else{
					baseLocale = baseLocaleOrg.replaceFirst(script[0], scriptMappingformBList.get(scriptMappingformAList.indexOf(script[0])));
				}
				step = 9;
			} else if((script.length>0 && macroLanguageformAList.contains(script[0]))){ // Step 7
				baseLocale = baseLocaleOrg.replaceFirst(script[0], individualLanguageFormC[macroLanguageformAList.indexOf(script[0])]);
				step = 9;
			} else {
				baseLocale = baseLocaleOrg;
			}
			
			boolean gotCompleteResult = false;
			while(!gotCompleteResult){
				hasScript = hasCountry = false;
				switch(step){
				case 9:
					resultCandidateList.add(baseLocale);
				case 10:
					//Step 10
					baseLocaleTemp = baseLocale;
				    numberOFUnderscore = baseLocaleTemp.length()-baseLocaleTemp.replaceAll("_", "").length();
					script = baseLocaleTemp.split("_", 4);
				case 11:
					//Step 11
					resultCandidateList.add(baseLocaleTemp);
					String candidateList = null;
					numberOFUnderscore = baseLocaleTemp.length()-baseLocaleTemp.replaceAll("_", "").length();
					if(numberOFUnderscore>=2){
						if(script[1].length()==4){
							//Script is there in the string
							hasScript = true;
							if(numberOFUnderscore>=3 && (script[2].length()==2 || script[2].length()==3)){
								//country tag is present
								indexOfVariant = baseLocaleTemp.indexOf(script[3]);
								hasCountry = true;
								indexOfCountry = baseLocaleTemp.indexOf(script[2]);
							} else {
								//country tag is not present
								if(numberOFUnderscore>=3){
									indexOfVariant = baseLocaleTemp.indexOf(script[3]);
								}else{
									indexOfVariant = 0;
								}
							}
						}  else if(numberOFUnderscore>=3 && script[1].equals("") && (script[2].length()==2 || script[2].length()==3 || script[2].length()==3 )){
							hasScript = true;
						}else {
							if(script[1].length()==2 || script[1].length()==3){
								hasCountry = true;
								indexOfCountry = baseLocaleTemp.indexOf(script[1]);
							}
							indexOfVariant = baseLocaleTemp.indexOf(script[2]);
						}
						if(!(indexOfVariant==0)){
							String variantCandidateList = baseLocaleTemp.substring(indexOfVariant, baseLocaleTemp.length());
							
							while(variantCandidateList.length()!=0){
								if(variantCandidateList.contains("_")){
									variantCandidateList = variantCandidateList.substring(0, variantCandidateList.lastIndexOf("_"));
									candidateList = baseLocaleTemp.replaceAll(baseLocaleTemp.substring(indexOfVariant, baseLocaleTemp.length()), variantCandidateList);
								} else {
									candidateList = baseLocaleTemp.substring(0, baseLocaleTemp.lastIndexOf("_"));
								        while(candidateList.charAt(candidateList.length()-1)=='_'){
										candidateList = candidateList.substring(0, candidateList.length()-1);
									}      
                                                                 	variantCandidateList = "";
								}
								resultCandidateList.add(candidateList);
							}
						}
						
					}
					
					if(script.length>1 && script[1].length()==4){
						hasScript = true;
						if(script.length>2 && (script[2].length()==2 || script[2].length()==3)){
							hasCountry = true;
							indexOfCountry = baseLocaleTemp.indexOf(script[2]);
						}
					}else{
						if(script.length>1 && (script[1].length()==2 || script[1].length()==3)){
							hasCountry = true;
							indexOfCountry = baseLocaleTemp.indexOf(script[1]);
						}
					}
									
					// Step 12
					if(hasCountry){
						candidateList = baseLocaleTemp.substring(0, indexOfCountry-1);
						resultCandidateList.add(candidateList);
					}else{
						if(!hasScript){
							candidateList = baseLocaleTemp.substring(0, 2);
							resultCandidateList.add(candidateList);
						}
					}
					
					// Step 13
					if(hasScript){
						baseLocaleTemp = baseLocale;
						baseLocaleTemp = baseLocaleTemp.replaceFirst("_"+script[1], "");
						script = baseLocaleTemp.split("_", 4);
						step = 11;
						break;
					}
					
					//Step 14
					String languageToReplace;
					if(macroLanguageformAtable.containsKey(languageToReplace=baseLocaleTemp.substring(0, 2))){
						baseLocaleTemp = baseLocale;
						baseLocaleTemp = baseLocaleTemp.replaceFirst(languageToReplace ,(String)macroLanguageformAtable.get(languageToReplace));
						script = baseLocaleTemp.split("_", 4);
						step = 11;
						break;
					}
					
					if(candidateList == null || (candidateList.length()==2 && hasScript == false)|| candidateList.equals("")){
						gotCompleteResult = true;
					}
				};
			}
			
			List<String> resultCandidateListTemp = new ArrayList<String>();
			Iterator i = resultCandidateList.iterator();
			while(i.hasNext()){
				String temp;
				if(!resultCandidateListTemp.contains(temp=(String)i.next())){
					resultCandidateListTemp.add(temp);
				}
			}
			resultCandidateList.clear();
			resultCandidateList = resultCandidateListTemp;
		//}
		ext = extension;
		if(ext!=null){ //only for Demo
			resultCandidateListTemp.add("@");
			resultCandidateListTemp.add(ext);
		}
		return resultCandidateList.toArray(new String[0]);
	}

        public static String[] getJDKLocales(String locale){

		JDKCandidateList.clear();
		String tempLocale = locale;

		if(tempLocale.contains("@")){

			tempLocale = tempLocale.substring(0, tempLocale.indexOf('@')-1);

		}

		JDKCandidateList.add(tempLocale);

		while(tempLocale.contains("_")){

			tempLocale = tempLocale.substring(0, tempLocale.lastIndexOf('_'));

			if(tempLocale.charAt(tempLocale.length()-1)=='_'){

				while((tempLocale.charAt(tempLocale.length()-1)=='_')){

					tempLocale = tempLocale.substring(0, tempLocale.lastIndexOf('_'));

				}

			}

			JDKCandidateList.add(tempLocale);

		}

		return JDKCandidateList.toArray(new String[0]);

	}		

		
}
		
			

		

