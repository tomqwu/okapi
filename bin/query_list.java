//Date: July 22, 2007
//Author: Petras Gorys
/*Input: User chooses the query files at runtime, the database, and whether they want to view
some or all of the data being managed/returned*/
//Output: The data being processed and the results of the query search
//Version: 1.08
/*Version History:
1.00 - Take a words from a predefined text file and output it to the screen
1.01 - Take a word from an predefined text file and perform a serch on it
1.02 - Take a word from a text file specified in the program argument list and perform 
a search on it
1.02_01 - Take words from String variables, perform a search on them, and perform
a search on the combination of all three
1.03 - As 1.02_01 but output the result list to the screen
1.04 - As 1.03 but the words are taken from a text file in the argument list
1.05 - As 1.04 but given the option to end the program before processing the data
1.06 - As 1.05 but give the option to view or not view the data being processed
1.07 - As 1.06 but give the option to display one or more results from the result list
1.08 - As 1.07 but can accept words from multiple text files
*/

import java.util.*;
import java.io.*;
import java.lang.*;
import java.util.regex.*;



//The purpose of this class is to read a query from a file, interpret it
//modify it to be understood by the Okapi Basic Search System, and send
//it to the BSS to initiate a query.

public class query_list {

	public query_list(String[] list) throws Exception {
	
		String out = "Welcome to the Okapi Information Retrieval Engine.\n";
		javokapi okapi = new javokapi();
	
		System.out.println(out);
		
		System.out.println("The databases available to you are: " + okapi.infoDB());
		System.out.println("Please choose one.");
		BufferedReader db = new BufferedReader(new InputStreamReader(System.in));
		String databaseName = db.readLine();
				
		//Choose the database to use
		//ch databaseName
		okapi.chooseDB(databaseName);
		//Delete all previous set data
		//delete all
		okapi.deleteAllSets();
		System.out.println("The database you are you using is: " + databaseName);

		String file[] = new String[list.length];
		
		
		//Read through the list of filenames and extract their name data
		for (int i = 0; i < list.length; i++) {
			file[i] = list[i];
		}
		
		System.out.print("The system will now try and read the data from: ");
		for (int i = 0; i < file.length; i++) {
			System.out.print(file[i] + " ");
		}
		System.out.println("\nIs this alright? (y/n)");
		BufferedReader read = new BufferedReader(new InputStreamReader(System.in));
		String check = read.readLine();
		while (true) {
			if (check.equals("y")) {
				readFile(okapi, file, list.length);
				break;
			}
			else if (check.equals("n")) {
				System.out.println("Ending program.");
				break;
			}
			else {
				read = new BufferedReader(new InputStreamReader(System.in));
				check = read.readLine();
			}
		}
	
	
	}
	
	private void readFile(javokapi okapi, String[] file, int files) throws Exception {
		String wrklist = "";
		FileReader fread;
		BufferedReader bufread;
				
		//Try: read the file and extract all query term data from it
		try {
			for (int i = 0; i < files; i++) {
				
				boolean flag = true;
				fread = new FileReader(file[i]);
				bufread = new BufferedReader(fread);
				while (flag) {
					String temp = bufread.readLine();					
					if (temp != null)
						wrklist += temp;
					else
						flag = false;					
				}
			}
		}
		catch (FileNotFoundException e) {
			System.out.println("One or more files could not be read");
		}		
		catch (Exception e) {
			System.out.println(e);
		}
		
		System.out.println("Would you like to view all the results of the search or not? (all, none)");
		
		BufferedReader response = new BufferedReader(new InputStreamReader(System.in));
		String quietverb = response.readLine();		
		
		if (quietverb.equals("all")) {
			System.out.println("\nQuery terms: " + wrklist + "\n");
			//Superparse will scan through the index file and look for any use of the word or its derivitives of the terms in the query list
			System.out.println("The system will now look for the query terms in the index.");
		}

		//String complete = "gene disease role prnp";
		
		//superparse t=wrklist
		String sparselist = okapi.superParse("", wrklist);
		//String sparselist = okapi.superParse("", complete);
		
		if (quietverb.equals("all")) {
			System.out.println("Parsed Terms: " + sparselist);
			System.out.println("Note: Duplicate terms will result in imbalanced weighing.");
		}
		
		//This Linked List will point to a list of query terms extracted from the Okapi generated superparse list
		LinkedList termlist = findT(sparselist);
		
		//This string is used to hold the collection of each term's weight
		String weights[] = new String[termlist.size()];
		String weightlist = "";
		int count = 0;
		
		while (termlist.size() != 0) {
			//Remove the first term from the list
			String term = (String)termlist.removeFirst();
			//Uses the simple Okapi Find command to retrieve the number of positings for the term
			//find  save=1 attribute=DEFAULT  t=term
			String flist = okapi.find("1","0","DEFAULT", term);
			//Extract the number of postings from the Okapi Find result
			String np = findNP(flist, term);
			//Use the Okapi Weight command to find the weight of the term
			//weight fn=2 n=np r=0 bigr=0 rload=4 bigrload=5
			weights[count] = okapi.weight("2",np,"0","0","4","5");
			
			if (quietverb.equals("all")) {
				System.out.print("Term Information: " + flist);
				System.out.println("Term Weight: " + weights[count]);
			}
			
			//Format the weightlist so that it can be used in conjunction with the existing setFind javokapi method
			if (count == 0) {
				weightlist = weights[count];
			}
			else
				weightlist += "s=" + count + " w=" + weights[count];
			count++;
		}
		//Determine the number of relevant documents as well as the the weight of the most relevant document
		//find s=0 w=weights[0] s=1 w=weights[1] ... 
		String fullfind = okapi.setFind("0", weightlist, "bm25");
		
		if (quietverb.equals("all")) {
			System.out.println("Full Details: " + fullfind);
		}
		
		int npInt = lastNP(fullfind);
		
		System.out.println("There are " + npInt + " relevant documents.");
		System.out.println("How many would you like to view? (Enter any number from 1- [# of documents] or 'all')");
		
		BufferedReader input = new BufferedReader(new InputStreamReader(System.in));
		String sinput = input.readLine();
		int docs = npInt+1;
		if (!sinput.equals("all")) {
			docs = Integer.parseInt(sinput);
		}
		
		while (true) {
			if (docs <= npInt) {
				//Output each relevant document found in the format of: Record #, PMID, Document Weight
				for (int i = 0; i < docs; i++) {
					System.out.print("Record # " + i + ": ");
					System.out.println(okapi.showSetRecord("197", "", Integer.toString(i), ""));
				}		
				break;
			}
			else if (sinput.equals("all")) {
				//Output each relevant document found in the format of: Record #, PMID, Document Weight
				for (int i = 0; i < npInt; i++) {
					System.out.print("Record # " + i + ": ");
					System.out.println(okapi.showSetRecord("197", "", Integer.toString(i), ""));
				}
				break;
			}
			else {
				input = new BufferedReader(new InputStreamReader(System.in));
				if (!sinput.equals("all")) {
					docs = Integer.parseInt(sinput);
				}
			}
		}
			
			
	}
	
	
	private LinkedList findT(String termList) {
	//This method removes all excess generated by Okapi and returns a list of root words found in Okapi from the original query list
		String revisedlist = termList;
		String tip = "", tap = "";
		
		boolean tequalsflag = false;
		LinkedList secondString = new LinkedList();

		for (int i = 0; i < revisedlist.length(); i++) {
			//Search for t= to find the start position of the term name
			if (i < termList.length()-1) { 
				tap = revisedlist.substring(i, i+1); 
				if (tap.equals("t")) {
					tip = revisedlist.substring(i+1, i+2);
					if (tip.equals("=")) {
						tequalsflag = true;
					}
					if (tequalsflag) {
						for (int j = i; j < revisedlist.length(); j++) {
							tap = revisedlist.substring(j, j+1);
							//When the "scan" has reached the end of the term name, copy the term name to the LinkedList
							if (tap.equals(" ")) {
								secondString.add(revisedlist.substring(i+2, j));
								break;
							}
						}
					}
					tequalsflag = false;
				}	
			}
		}
				
		return secondString;
	}
	
	private int lastNP(String lastFind) {
	//This method removes all the excess output generated by Okapi and returns an integer giving the value of np
	//This method is used only on the final Okapi Find result because I couldn't get findNP compatible with it
		String scan1 = "", scan2 = "", scan3 = "", scan4 = "";
		String result = "";
		
		for (int i = 0; i < lastFind.length(); i++) {
			if (i < lastFind.length()-1) {
				scan1 = lastFind.substring(i, i+1);
				if (scan1.equals("n")) {
					scan2 = lastFind.substring(i+1, i+2);
					if (scan2.equals("p")) {
						scan3 = lastFind.substring(i+2, i+3);
						if (scan3.equals("=")) {
							for (int j = i+3; j < lastFind.length(); j++) {
								scan4 = lastFind.substring(j, j+1);
								if (scan4.equals(" ")) {
									result = lastFind.substring(i+3, j);
									break;
								}
							}
						}
					}
				}
			}
		}
                if ("".equals(result)) { return 0; }
		int temp = Integer.parseInt(result);
		
		return temp;
	}
	
	private String findNP(String toSplit, String queryTerm) {
	//THIS METHOD REMOVES THE EXCESS OUTPUT GENERATED BY OKAPI AND RETURNS ONLY THE VALUE OF NP
		
		//--------------THIS CODE REMOVES THE "SO np=" PART GENERATED BY OKAPI

		//Create a pattern to match cat
		Pattern p = Pattern.compile("S\\d np=");

		//Create a matcher with an input string
		Matcher m = p.matcher(toSplit);

		StringBuffer sb = new StringBuffer();
		boolean result = m.find();

		//Loop through and create a new String with the replacements
		while (result) {
			m.appendReplacement(sb, "");
			result = m.find();
		}
	  
		//Add the last segment of input to the new String
		m.appendTail(sb);

		//System.out.println(sb.toString());
		String secondSplit = sb.toString();

		//--------------THIS CODE REMOVES THE "t=" PART GENERATED BY OKAPI

		Pattern p2 = Pattern.compile("t=");
	 
		//Create a matcher with an input string
		Matcher m2 = p2.matcher(secondSplit);
	  
		StringBuffer sb2 = new StringBuffer();
		boolean result2 = m2.find();

		//Loop through and create a new String with the replacements
		while (result2) {
			m2.appendReplacement(sb2, "");
			result2 = m2.find();
		}
	  
		//Add the last segment of input to the new String
		m2.appendTail(sb2);
		String thirdSplit = sb2.toString();

		//--------------THIS CODE REMOVES THE "term searched" GENERATED BY OKAPI

		Pattern p3 = Pattern.compile(queryTerm);

		//Create a matcher with an input string
		Matcher m3 = p3.matcher(thirdSplit);

		StringBuffer sb3 = new StringBuffer();
		boolean result3 = m3.find();

		//Loop through and create a new String with the replacements
		while (result3) {
			m3.appendReplacement(sb3, "");
			result3 = m3.find();
		}
	  
		//Add the last segment of input to the new String
		m3.appendTail(sb3);
		String finalResult = sb3.toString();
		finalResult = finalResult.trim();

		return finalResult;
	}

	
	public static void main (String[] args) throws Exception {
		query_list ql = new query_list(args);
			
	}


}
