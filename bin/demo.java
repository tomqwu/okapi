import java.util.*;
import java.io.*;
import java.lang.*;
/**
 * To demonstrate the functionality of javokapi package.
 * 
 * @author (Hashmatullah Rohian	) 
 * @version (September 06)
 */
public class demo
{
    public demo() throws Exception{
        String out = "Welcome to Javokapi Demo program.\n";
        javokapi okapi = new javokapi();
        
        /*Given the name of the Java program responsible for generating exchange format of the given corpus, compiles the Java program and generates the exchange format using Piccolo XML parser named output into the parse directory.
        */
        System.out.println(okapi.exchangeFormat("gen_parser","trec04", "gen_exch"));
        /*Given the name of main parameter file responsible for generating runtime format, converts the already parsed data and generates the runtime format into the bibfiles directory. In case, the Javokapi package is unable to handle these functions, they can be manually called by executing the shell command "sh X", where X is the appropriate function being used.
        */okapi.runtimeFormat("2004gendoc", "gen_exch");
        /*Given the name of the main parameter file responsible for generating index of the runtime format, converts the already parsed data and generates the runtime format into the bibfiles directory. In case, the Javokapi package is unable to handle these functions, they can be manually called by executing the shell command "sh X", where X is the appropriate function being used.
        */
        okapi.createIndex("2004gendoc",100, true, true, 0);
        /*
        infoDB requests information about the available databases.
        */
	System.out.println(okapi.infoDB());
	for (int i=0; i < 10; i++)
	{  	
		String count = Integer.toString(i);
		System.out.println(okapi.showRecord("197", count));
		System.out.println(count);
	}
		
	


System.out.println(okapi.chooseDB("2004gendoc"));
        /* Setting default parameters
        */
		System.out.println(okapi.setDefaultK1(1.4));
		System.out.println(okapi.setDefaultK2(0.0));
		System.out.println(okapi.setDefaultK3(8.0));
		System.out.println(okapi.setDefaultBM25_number(0.55));
		System.out.println(okapi.setDefaultOptions("bm25"));
		
		File testFile = new File("qadhoc05"); 
        BufferedReader input = null;

        input = new BufferedReader(new FileReader(testFile));
        String line = null; 
		String[] parts={};
		int queryNo=0;
		HashSet mainTerms, feedbackTerms;
		mainTerms = new HashSet();
        feedbackTerms = new HashSet();
        /*
        	Given the structured query file, for every query separate the main query terms and feedback terms and collect in a set
        */
        while ((line = input.readLine()) != null) {
        	if (line.trim().equals("")) {
	           processQuery(queryNo,okapi,mainTerms, feedbackTerms);
	           //System.out.println(queryNo+" M::"+mainTerms + " F::" + feedbackTerms);
               continue;
            }
            if (line.trim().startsWith("<ID>")){
	            parts = line.split("</ID>");
            	queryNo = Integer.parseInt(parts[0].replaceAll("<ID>", ""));
           		mainTerms = new HashSet();
            	feedbackTerms = new HashSet();
            	continue;
       		}
            if (line.contains("$")){
			    String temp = line.replaceAll("\\$.*\\$", "");
		        feedbackTerms.add(temp.trim());
	        }else{   
	        	mainTerms.add(line.trim());
            }
            		
        }       
     	processQuery(queryNo, okapi, mainTerms, feedbackTerms);
     	//System.out.println(queryNo+" M::"+mainTerms + " F::" + feedbackTerms);
	}
	/*
		Given a query a number, a javokapi object and the set of main query terms and feedback terms. The
		original input file is qadhoc05 and the feedback discount is 0.11.
	*/
	private void processQuery(int qryno, javokapi okapi, HashSet mainSet, HashSet feedSet) throws Exception{
		String mainResult = processMain(okapi, mainSet);
		String feedResult = processFeedback(okapi, feedSet);
		//the default value of k1 is 1.2 and bm25_b is 0.75, why not use default value?   
        String	result = okapi.setFind(mainResult + feedResult,"", "bm25 k1=1.4 bm25_b=0.55");
                
        String[] parsed = result.split(" maxwt=");
        String[] parsed2 = parsed[0].split(" np=");     //number of papers 
        
        int totaldoc = new Integer(parsed2[1]).intValue(); 
        StringBuffer final_result_doc = new StringBuffer();
      
        for (int i = 0; i < java.lang.Math.min(totaldoc, 10); i++) {
	        result = okapi.showSetRecord("197", "1", "","");
            StringTokenizer pair1 = new StringTokenizer(result);
            //System.out.println(result);
            String docnum = pair1.nextToken();
            Float weig = new Float(pair1.nextToken());

            final_result_doc.append(
                    new Integer(qryno) + " Q0 " + docnum + " "
                    + new Integer(i + 1) + " " + weig + " " + "yorku_doc\n");           
        }
        BufferedWriter output = new BufferedWriter(new FileWriter(""+qryno));   //write into result\queryNumber

        output.write(final_result_doc.toString());
        output.close();
	}
	/* Given a okapi object and the list of feedback terms, process the terms and obtain the set numbers from
	the calculations.
	*/
	private String processFeedback(javokapi okapi, HashSet feedbackSet){
		final float discount = 0.11f;
		
		Iterator itFeedback = feedbackSet.iterator();
		
		StringBuffer phraselist = new StringBuffer();
        StringBuffer dellist = new StringBuffer();
		String setResult = "";        
        while(itFeedback.hasNext()){
			String sentence = (String) (itFeedback.next());
            StringTokenizer word = new StringTokenizer(sentence, "+");
            while (word.hasMoreTokens()){
	        	String phrase1 = word.nextToken();
            	StringTokenizer ph = new StringTokenizer(phrase1);
           	 	StringBuffer termlist = new StringBuffer();

            	while (ph.hasMoreTokens()) {
                	String preterm = ph.nextToken().trim();
                	String result = okapi.superParse("",preterm);                         
                	String[] parsed = result.split("t=");	
                	String[] parsed2 = parsed[1].split(" c=");
                	String term = parsed2[0];

                	if (preterm.startsWith("&")) { 
                    	term = preterm.replaceAll("&", "");
                	}
                
                	String findresult = okapi.find("","","",term);
                	parsed = findresult.split(" np=");
                	parsed2 = parsed[0].split("S");
                	String setnum = parsed2[1];

                	parsed2 = parsed[1].split(" t=");
     
    	            dellist.append(" s=" + setnum);
        	        termlist.append(" s=" + setnum);
				
            	}
                                   
            	String stTermList = termlist.toString();                   
            	String result = okapi.setFind(stTermList, "","adj");
            	String[] parsed = result.split(" np=");
            	String[] parsed2 = parsed[0].split("S");
            	String setnum = parsed2[1];

            	dellist.append(" s=" + setnum);
            	phraselist.append(" s=" + setnum);
			
        	}
			String stPhraseList = phraselist.toString();
        	String result = okapi.setFind(stPhraseList,"", "or");
        	String[] parsed = result.split(" np=");
        	String[] parsed2 = parsed[0].split("S");
        	String setnum = parsed2[1];
        	String totalnp = parsed[1].trim();

        	result = okapi.weight("",totalnp, "0","0","","");
        	float weight = new Float(result).floatValue();
        	weight = weight * discount;
 
        	okapi.deleteRecord(dellist.toString());         	          
        	setResult = setResult + "s=" + setnum + " w="
                    + weight + " ";
     	}
     	return setResult;
 	}
	/* Given a okapi object and the list of main query terms, process the terms and obtain the set numbers from
	the calculations.
	*/
	private String processMain(javokapi okapi, HashSet mainSet){
		Iterator itMain = mainSet.iterator();
		
		StringBuffer phraselist = new StringBuffer();
        StringBuffer dellist = new StringBuffer();
		String setResult = "";        
        while(itMain.hasNext()){
			String sentence = (String) (itMain.next());
            StringTokenizer word = new StringTokenizer(sentence, "+");
            while (word.hasMoreTokens()){
	        	String phrase1 = word.nextToken();
            	StringTokenizer ph = new StringTokenizer(phrase1);
           	 	StringBuffer termlist = new StringBuffer();
				//System.out.println(phrase1);
            	while (ph.hasMoreTokens()) {
                	String preterm = ph.nextToken().trim();
                	String result = okapi.superParse("",preterm);                         
                	//System.out.println(preterm + ":" + result);
                	String[] parsed = result.split("t=");	
                	String[] parsed2 = parsed[1].split(" c=");
                	String term = parsed2[0];

                	if (preterm.startsWith("&")) { 
                    	term = preterm.replaceAll("&", "");
                	}
                
                	String findresult = okapi.find("","","",term);
                	//System.out.println(term + "::" + findresult);
                	parsed = findresult.split(" np=");
                	parsed2 = parsed[0].split("S");
                	String setnum = parsed2[1];

                	parsed2 = parsed[1].split(" t=");
     
    	            dellist.append(" s=" + setnum);
        	        termlist.append(" s=" + setnum);
				
            	}
            	
                String stTermList = termlist.toString();                   
            	String result = okapi.setFind(stTermList, "","adj");
            	//System.out.println(stTermList.trim()+ ":::" + result);
            	String[] parsed = result.split(" np=");
            	String[] parsed2 = parsed[0].split("S");
            	String setnum = parsed2[1];

            	dellist.append(" s=" + setnum);
            	phraselist.append(" s=" + setnum);
			
        	}
		
        	String stPhraseList = phraselist.toString();
        	String result = okapi.setFind(stPhraseList,"", "or");
        	String[] parsed = result.split(" np=");
        	String[] parsed2 = parsed[0].split("S");
        	String setnum = parsed2[1];
        	String totalnp = parsed[1].trim();

        	result = okapi.weight("",totalnp, "0","0","","");
        	float weight = new Float(result).floatValue();
 
        	okapi.deleteRecord(dellist.toString());         	          
        	setResult = setResult + "s=" + setnum + " w="
                    + weight + " ";
     	}
     	return setResult;
 	}

 	public static void main(String[] args) throws Exception{
	 	demo dm = new demo();
 	}
}

