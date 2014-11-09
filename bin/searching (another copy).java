//Date: August 13, 2007
//Last Modified: August 17, 2007
//Author: Petras Gorys
/*Input: User chooses the query files at runtime, the database, and whether they want to view
some or all of the data being managed/returned*/
//Output: The data being processed and the results of the query search into a file called query-responseX.txt
//First Release Version: 1.03
//Current Version: 1.03
/*Version History:
1.00 - Modify searching.java to remove unnecessary/inadaptable material
1.01 - As 1.00 but is able to take input in a new format, write it to a query list, and then ouput
the structure of the query list
1.02 - As 1.01 but is able to read the query list and implement pseudo find commands
1.03 - As 1.02 but is able to use real javokapi and Okapi find commands
*/

import java.util.*;
import java.io.*;
import java.lang.*;
import java.util.regex.*;

//The purpose of this class is to read a query from a file, interpret it
//modify it to be understood by the Okapi Basic Search System, and send
//it to the BSS to initiate a query.
public class searching{
	/*This variable acts as a counter that tracks the current set number 
	so that Javokapi SetFind statements know which Okapi Find are associated
	with them*/
	private int set_value = 0;
	/*Keeps track of the number of results that come from the final SetFind
	that binds all the query terms and statements so that the output file constructor 
	knows how many result lines to print to file*/
	private int final_np = 0;
	//Keeps the directory identity for where the results of the Okapi search are kept
	private String OKAPI_QUERYDIR = "/home/irlab/javokapi/results/";

	/*The constructor handles the user interface as it relates to choosing a database and
	making the final decision about whether the query file(s) entered at runtime are correct */
	public searching(String[] list) throws Exception {
	
		String out = "\nWelcome to the Okapi Information Retrieval Engine.\n";
		String file[] = new String[list.length];
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
		
		//Read through the list of filenames and extract their name data
		for (int i = 0; i < list.length; i++) {
			file[i] = list[i];
		}
		
		System.out.print("The system will now try and read the data from: ");
		for (int i = 0; i < file.length; i++) {
			System.out.print(file[i] + " ");
		}
		System.out.println();
		System.out.println("\nIs this alright? (y/n)");
		BufferedReader read = new BufferedReader(new InputStreamReader(System.in));
		String check = read.readLine();

		while (true) {
			if (check.equals("y")) {
				runProgram(okapi, file, list.length);
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
	/*This method reads through the file(s), takes their query data. and passes it along to methods which put it into a hierarchical list,
	read through the hierarchy to call Okapi Find and Weight methods, and then output the results to text files*/
	private void runProgram(javokapi okapi, String[] file, int files) throws Exception {
		String wrklist = "", temp = "";
		int line_count = 0;
		boolean flag = true, make_dir = false;;
		FileReader fread;
		BufferedReader bufread;
		query query_list;
				
		try {
			/*Attempts to create a folder in the Javokapi directory called /results/ in which all the
			query results from the Okapi Find are kept*/
			make_dir = (new File(OKAPI_QUERYDIR)).mkdir();
			if (make_dir) {
				System.out.println("Directory /javokapi/results/ created.\n");
			}
			else {
				System.out.println("Directory /javokapi/results/ could not be created or already exists.\n");
			}
			//Read through the list of files
			for (int i = 0; i < files; i++) {			
				flag = true;
				fread = new FileReader(file[i]);
				bufread = new BufferedReader(fread);
				//While there are still queries in the input file(s) continue to process them
				while (flag) {
					temp = bufread.readLine();					
					if (temp != null) {
						wrklist = temp;
						System.out.println(wrklist + "\n");
						query_list = prepareQueryList(wrklist, line_count);
						measureQueryList(okapi, query_list.getLeftNode());
						createOutput(okapi, query_list.getTerm(), final_np);
						line_count++;
						
					}
					else {
						flag = false;					
					}
				}
			}
		}
		catch (FileNotFoundException e) {
			System.out.println("One or more files could not be read");
		}		
		catch (Exception e) {
			System.out.println(e);
		}
	}
	/*This method takes data from the indexed records, formats it, and outputs this formatted data to a text file which is
	labelled based on the position it held in the query text file*/
	private void createOutput(javokapi okapi, String topic_id, int np) {
		FileWriter fwrite = null;
		BufferedWriter buffwrite = null;
		String data = "", sorted_data = "", rec_count = ""; 
		String[] data_parts = new String[2];
		
		try {
			/*The first line of the results text file is the original search parameters so that readers looking at the results know
			where they came from*/
			fwrite = new FileWriter(OKAPI_QUERYDIR + "topic-" + topic_id + "-results.txt");
			buffwrite = new BufferedWriter(fwrite);

			/*Each file is limited to presenting the first 30 results of a query search.
			Some query result lists may exceed 100 entries which can cause file creation to take a great
			deal of time and so you should adjust this number based on how many results you want to see*/
			if (np >= 1000) {
				np = 1000;
			}
			/*Extract the PMID and Document Weight from the 197 output option while taking the Desctipion information
			from the full indexed text*/
			for (int i = 0; i < np; i++) {
				rec_count = "" + i;
				data = okapi.showSetRecord("197", "", rec_count, "");				
				data_parts = data.split(" ");
				data_parts[1] = data_parts[1].trim(); 
				sorted_data = topic_id + " Q0 " + data_parts[0] + " " + (i) + " " + data_parts[1] + " pgorys";
				
				
				//Tom
				//buffwrite.write(data);
				
				buffwrite.write(sorted_data);	
				buffwrite.newLine();
				
			}
			buffwrite.close();
		}
		catch(IOException e) {
			System.out.println("IOException: " + e);
		}
		catch(Exception e) {
			System.out.println("Exception: " + e);
		}				
	}
	
	/*Takes the list of query terms formatted in a query list, performs an Okapi find on each individual term and
	term set, then combines each set number and term (set) weight together to calculate the final find result*/
	private void measureQueryList(javokapi okapi, query list) {
		int count = 0;
		priority_query single_group;
		query last_entry = new query();
		String first_set = " ", first_weight = "", weight_list = "";
		boolean start = true;
		
		while (list != null) {
			/*Perform a find on a single term/term group that is contained within a single set of commas and return the
			priority_query object which contains information gained when the query term/group was searched and weighed
			as well as the number of entries that were traversed so that the list in this method can "catch up"*/
			single_group = measureQuery(okapi, null, null, list, false, count);
			for (int i = count; i < single_group.getCount(); i++) {
				if (list != null) {
					list = list.getLeftNode();	
				}	
			}
			//Reset the count so that the traversal performed above isn't done too many times
			single_group.setCount(count);
			//Compile a list of set numbers and term weights which will then be used as part of the final bm25 search
			for (query temp = single_group.getQueryList(); temp != null; temp = temp.getLeftNode()) {
				if (temp.getLeftNode() == null) {
					if (start) {
						first_set = temp.getSetNumber();
						first_weight = temp.getWeight();
						
						last_entry.setNumber(first_set);
						last_entry.setWeight(first_weight);
						
						start = false;
					}
					else {
						weight_list += " s=" + temp.getSetNumber() + " ";
						weight_list += "w=" + temp.getWeight();
					}
				}
				last_entry.setTerm(last_entry.getTerm() + temp.getTerm() + " ");
			}
		}
		last_entry.setWeight(last_entry.getWeight() + weight_list);
		makeFind(okapi,last_entry, set_value, 2);

	}
	/*Recursively moves through the query term list (if there are adjacent terms) in order to find and weigh query terms/groups and returns
	a new priority_query object back containing the results so that they can be processed*/
	private priority_query measureQuery(javokapi okapi, priority_query new_list, query curr, query list, boolean adj, int count) {		
		/*When the reading of the adjacent term list is complete then the recursive solution is made to go backwards so that
		the makeFind method can be called on the query terms and placed into a new priority_query object*/
		if (list == null) {
			System.out.println("null list");
			return new_list;
		}
		/*When first encountering an adjacency create a new prority_query object and recursively move through the existing list.
		When the recursion is complete and it returns to this point, perform a find on the terms and then a find using the adjacency makeFind option*/
		else if (list.getTerm().equals("aS")) {
			new_list = new priority_query();
			new_list.setQueryList(new query());
			new_list.setCount(++count);
			measureQuery(okapi, new_list, new_list.getQueryList(), list.getLeftNode(), true, count);
			
			for (query temp = new_list.getQueryList(); temp != null; temp = temp.getLeftNode()) {
				if (!temp.getTerm().equals("")) {
					makeFind(okapi, temp, set_value, 0);
					set_value++;
				}
			}
			makeFind(okapi, new_list.getQueryList(), set_value, 1);
			set_value++;
		}
		//When the end of the adjacency is found increase the counter and let the return statement be initialized
		else if (list.getTerm().equals("aF")) {
			new_list.setCount(++count);
		}
		/*If the method is recursively told that it is looking at an adjacency, create a new blank space and then recursively
		move forward. When the recursion comes back, place the current term into the empty list space*/
		else if (adj == true) {
			new_list.setCount(++count);
			curr.setLeftNode(new query());
			measureQuery(okapi, new_list, curr.getLeftNode(), list.getLeftNode(), true, count);
			curr.setTerm(list.getTerm());
		}
		//If the method is not looking at an adjacency then perform a standard find on the query term and return the results
		else if (adj == false) {
			query copy;
			
			new_list = new priority_query();
			makeFind(okapi, list, set_value, 0);
			set_value++;
			
			copy = list.getShallowCopy();
			new_list.setQueryList(copy);
			new_list.setCount(++count);
			
		}
		return new_list;
	
	}
	
	/*Utilizes the Okapi and Javokapi functions to perform a search and weighing of various terms and collections of terms
	held together by boolean conditionals*/
	private void makeFind(javokapi okapi, query list, int s_value, int option) {
		String find_results = "", super_parse = "", np = "", 
		weight = "", find_list = "", first_number = "", query_term = "";
		LinkedList parsed;
	
		/*If the passed query object is a term then a conventional search and weighing is performed with all
		relevant data being stored in the term's query object*/
		if (option == 0) {
				System.out.println("term " + list.getTerm());
				super_parse = okapi.superParse("", list.getTerm());
				System.out.print("Super Parsed Term: " + super_parse + " ");
				parsed = findT(super_parse);
				query_term = (String)parsed.removeFirst();
				find_results = okapi.find("1","0","DEFAULT", query_term);
				System.out.print("Find Results: " + find_results);
				System.out.println(s_value + " " + set_value);
				list.setNumber("" + s_value);
				list.setNP(findNP(find_results));
				weight = okapi.weight("2",list.getNP(),"0","0","4","5");
				list.setWeight(weight.trim());		
				System.out.println("Weight: " + list.getWeight() + "\n");
				
		}
		/*If the passed query object is a set of terms then the query object is analyzed to produce a weight list
		so that an adjacency find can be performed. In all cases, the last member of the query list will be a query
		link which will be used to pass along the results of the adjacency find and weighing for use in further 
		searches*/
		else if (option == 1) {
			if (!list.equals("")) {
				first_number = list.getSetNumber();
				find_list += list.getWeight();
				System.out.print("Find and weigh term set " + list.getTerm() + " ");
						
				while (list.getLeftNode() != null) {
					list = list.getLeftNode();
					
					if (!list.getTerm().equals("")) {					
						find_list += " s=" + list.getSetNumber() + " ";
						find_list += "w=" + list.getWeight();
						System.out.print(list.getTerm() + " ");
					}		
				}
			}		
			System.out.println();
			System.out.println("s=" + first_number + " w=" + find_list + " op=adj\n");
			
			find_results = okapi.setFind(first_number, find_list, "adj");
			System.out.println("Total Find: " + find_results.trim());
			list.setNumber("" + s_value);
			list.setNP(findNP(find_results));
			weight = okapi.weight("2",list.getNP(),"0","0","4","5");
			list.setWeight(weight.trim());				
			System.out.println("Total Weight: " + list.getWeight() + "\n");
		}
		/*Performs the final find when called and takes a String  which contains all the information necessary to perform
		it. Additionally, since bm25 is merely an OR search using the bm25 weighing method, an and option is included to
		perform an AND search and then a weighing by using the bm25 formula*/
		else if (option == 2) {
			boolean addAnd = false;
			String start = list.getSetNumber();
			
			if (addAnd) {
				System.out.println("s=" + start + " w=" + list.getWeight() + " op=and");
				find_results = okapi.setFind(start, list.getWeight(), "and");
				System.out.print("Final Find: " + find_results);	

				start = set_value + "";
				np = findNP(find_results);
				weight = okapi.weight("2",np,"0","0","4","5");
				weight = weight.trim();
				System.out.println("Final Weight: " + weight + "\n");
				System.out.println("s=" + start + " w=" + weight + " op=and");
				find_results = okapi.setFind(start, weight, "bm25");
				System.out.println("Final Final Find: " + find_results);
				set_value++;
			}
			else {
				System.out.println("s=" + start + " w=" + list.getWeight() + " op=bm25");			
				find_results = okapi.setFind(start, list.getWeight(), "bm25");
				System.out.println("Final Find: " + find_results);	
				set_value++;
			}
			np = findNP(find_results);
			final_np = Integer.parseInt(np);
		}
	} 
	/*A throwback to the searching program which used this method more effectively. Calls the buildList method to
	take the text file query terms and create a query object which contains a specially formatted list of them to
	identify adjacency relationships. Returns the list so that it can be processed for the find command*/
	private query prepareQueryList(String line, int count) {
		int line_length = line.length();
		query final_list;
		
		final_list =  buildList(0, line, line_length);
		return final_list;
	}
	/*This method takes the query line which was read from the text source file and puts it into a query object
	list, all the while adding special formatting tags to distinguish when a term is part of an adjacency group
	or when it is a single term*/
	private query buildList(int i_pos, String line, int line_length) {
		query head, curr;
		String curr_char = "", char_list = "", comm_char = "";
		//adj_on is used to identify whether the current term is part of an adjacency when it is encountered
		/*once_per is used to identify the first use of the adjacency tag so it is not used again if
		more then one term is part of an adjacency relationship*/
		boolean adj_on = false, once_per = false;
		
		curr = head = null;
				
		while (i_pos < line_length) {
			/*Uses differenly sized substrings in order to determine when the current character belongs to a term
			or is a comma separator*/
			if (i_pos < line_length) {
				curr_char = line.substring(i_pos, i_pos+1);
				comm_char = "";
				if (i_pos < line_length-2) {
					comm_char = line.substring(i_pos, i_pos+2);
				}
			}
			//Create a new query object if there isn't one already
			if (curr == null) {
				head = new query();
				curr = head;
			}			
			//When the topic id container is encountered, increment the positional counter
			else if (curr_char.equals("<")) {
				i_pos++;			
			}
			/*When the topic id end container is encountered, increment the counter by two to skip
			over the special character and a space character. Create a new query link and set the focus
			to that new link*/
			else if (curr_char.equals(">")) {
				i_pos += 2;
				curr.setLeftNode(new query());
				curr = curr.getLeftNode();				
			}
			/*When a comma is encountered along with the space character then increment the counter
			to skip over them, create a new query link, move the method focus to it, and if the previous
			query term was part of an adjacency relationship, represent its conclusion with "aF", then
			create a new query link and set the focus to it. */
			else if (comm_char.equals(", ")) {
				i_pos += 2;

				System.out.println("comma aF");				
				curr.setLeftNode(new query());
				curr = curr.getLeftNode();
				
				if (adj_on) {
					curr.setTerm("aF");
					curr.setLeftNode(new query());
					curr = curr.getLeftNode();	
				}
				adj_on = false;
				once_per = false;
			}
			/*When an empty space is encountered then either an adjacency has first been
			discovered or one is being continued. If it is a new adjacency then create a
			space for a special adjacency identifying tag "aS", switch focus to the new tag,
			then create a new empty link and focus on that instead. If it is a continuing adjacency
			then only the new link and change of focus are performed instead*/
			else if (curr_char.equals(" ")) {
				i_pos++;
				adj_on = true;
				if (!once_per) {
					curr.setLeftNode(new query());
					curr.getLeftNode().setTerm(curr.getTerm());
					curr.setTerm("aS");
					curr = curr.getLeftNode();
					once_per = true;
				}
				curr.setLeftNode(new query());
				curr = curr.getLeftNode();
			}
			/*If this conditional is encountered, then the current character is part of a querying term and
			must be read completely to be processed. Characters are read and combined until a special character
			or a space character is encountered, forming the word. As long as there is an empty space available
			for it, then it is made lowercase and then put into it. The positional counter is moved forward a number
			of spaces equal to the length of the word to represent it having been read and the combined word
			String is made empty for the next use of this conditional*/
			else {
				int count = 0;
				for (count = i_pos; count < line_length; count++) {
					curr_char = line.substring(count, count+1);		
					
					if (!curr_char.equals(" ") &&
						(!curr_char.equals(",")) &&
						(!curr_char.equals("<")) &&
						(!curr_char.equals(">"))) {
							char_list += curr_char;
					}
					else {
						break;
					}
				}
				if (curr.getTerm().equals("")) {
					System.out.println(char_list);
					curr.setTerm(char_list.toLowerCase());
				}				
				i_pos = count;
				char_list = "";
			}
		}
		/*Should the length of the query character list be reached with the last term belonging to
		an adjacency, it is likely that the conditional that would put the representation of the adjacency
		relationship's finish was not encountered. If that is the case, then the boolean to represent that the
		adjacency is still active will be true, and so the representation is placed in a new query link before
		the returning of the entire list*/
		if (adj_on) {
			curr.setLeftNode(new query());
			curr.getLeftNode().setTerm("aF");
		}
		//printList(head);
		return head;
	}
	/*This method is used for debugging and was left for the same reason. It outputs the contents of the
	list built in buildList so that it can be analyzed for errors*/
	private void printList(query head) {
		while (head != null) {
			System.out.println(head.getTerm());
			head = head.getLeftNode();
		}
	}
	/*This method is used to extract as many query terms from the results of an Okapi SuperParse method as there exists. These
	terms are indicated to the method by the prefix "t=". The results are added to a Linked List for ease of addition and removal from
	a linear list to a linear call.*/
	private LinkedList findT(String termList) {
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
	/*This method takes the results from the Okapi Find method and extracts the number of postings (NP) that the search
	found of the query term*/
	private String findNP(String findResult) {
		String scan = "", np = "";
		int start_pos = 0, end_pos = 0;

		for (int i = 0; i < findResult.length(); i++) {
			scan = findResult.substring(i, i+1);
			if (scan.equals("n")) {
				scan = findResult.substring(i+1, i+2);
				if (scan.equals("p")) {
					scan = findResult.substring(i+2, i+3);
					if (scan.equals("=")) {
						start_pos = i+3;
						for (int j = start_pos; j < findResult.length(); j++) {
							scan = findResult.substring(j, j+1);
							if (scan.equals(" ") || j == findResult.length()-1) {
								end_pos = j;
								np = findResult.substring(start_pos, end_pos);
								break;
							}
						}
					}
				}
			}
		}
		return np;
	}
	public static void main (String[] args) throws Exception {
		searching s = new searching(args);
	}
}
