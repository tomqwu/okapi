import java.util.*;
import java.io.*;
import java.lang.*;
import java.util.regex.*;

//Original Author: RAYMOND NUQUI
//First Created: 1 May 2007
//Author: Petras Gorys
//Modified: 4 August 2007

//THIS IS A SIMPLE DEMONSTRATION OF THE METHODS IN JAVOKAPI.

public class build_index{
	
	public build_index() throws Exception{
        
		javokapi okapi = new javokapi();  //CREATES NEW INSTANCE OF JAVOKAPI
		String exch_file = "", db_param = "";
		
		System.out.println("\nWelcome to the Javokapi Index Builder\n");
		System.out.println("=====================================");
		System.out.println("PART 1 -- CREATING THE EXCHANGE FORMAT FILE\n");
		System.out.println("Do you have a source file? (y/n):");
		BufferedReader response = new BufferedReader(new InputStreamReader(System.in));
		String yncheck = response.readLine();
			
		if (yncheck.equals("y")) {
			System.out.println("Default Parse File is \"gen_parser\"");
			System.out.println("Parse File (Please DO NOT include File Extension):");
			response = new BufferedReader(new InputStreamReader(System.in));
			String parser = response.readLine();
			System.out.println("Source File:");
			response = new BufferedReader(new InputStreamReader(System.in));
			String source = response.readLine();
			System.out.println("Please choose the name of the Exchange Format File to be created:");
			response = new BufferedReader(new InputStreamReader(System.in));
			exch_file = response.readLine();
			System.out.println("\nConverting " + source + " to " + exch_file + " using " + parser + "...");
				
	        /*Given the name of the parse file which is responsible for parsing the source file, the exchange format file is created by removing all the
			XML tag data from the source file and replacing field separators with Okapi recognized spacers*/
			System.out.println(okapi.exchangeFormat(parser, source, exch_file));
		}
		else {
			System.out.println("SKIPPING CREATING THE EXCHANGE FORMAT FILE.");
		}

		System.out.println("=====================================");
		System.out.println("PART 2 -- CREATING THE RUNTIME FORMAT FILE\n");
		System.out.println("You cannot procede unless you have written a Database Parameter File.");
		System.out.println("Have you written a Database Parameter File? (y/n):");
		response = new BufferedReader(new InputStreamReader(System.in));
		yncheck = response.readLine();
		if (yncheck.equals("y")) {
			System.out.println("Database Parameter File:");
			response = new BufferedReader(new InputStreamReader(System.in));
			db_param = response.readLine();
			System.out.println("\nConverting " + exch_file + " using " + db_param + "...\n");
			
			/*Given the name of main parameter file responsible for generating runtime format, converts the already parsed data and generates the runtime format 
			into the bibfiles directory. In case, the Javokapi package is unable to handle these functions, they can be manually called by executing the shell 
			command "sh X", where X is the appropriate function being used.*/
			okapi.runtimeFormat(db_param, exch_file);
		}
		else {
			System.out.println("SKIPPING CREATING THE RUNTIME FORMAT FILE.");
		}		

		System.out.println("=====================================");
		System.out.println("PART 3 -- CREATING THE INDEX FILE(S)\n");
		System.out.println("You cannot procede unless you have written a Search Groups Parameter File.");
		System.out.println("Have you written a Search Groups Parameter File? (y/n):");
		response = new BufferedReader(new InputStreamReader(System.in));
		yncheck = response.readLine();
		if (yncheck.equals("y")) {
			System.out.println("How many index files are you creating?");
			response = new BufferedReader(new InputStreamReader(System.in));
			String index = response.readLine();
			int count_index = Integer.parseInt(index);
			long mem = 100;
			boolean delfinal = true;
			boolean doclens = true;
			long index_number = 0;
			
			System.out.println("The current settings will be for the " + index + " file(s) will be...");
			for (int list = 0; list < count_index; list++) {
				System.out.println("----------------------------------");
				System.out.println("DB Parameter File: " + db_param);
				System.out.println("Memory: " + mem);
				System.out.println("Delfinal=" + delfinal + " Doclens=" + doclens);
				System.out.println("Starting Index: " + list);
				System.out.println("----------------------------------");
			}
				
			System.out.println("Is this okay? (y/n):");
			response = new BufferedReader(new InputStreamReader(System.in));
			String change_settings = response.readLine();
			
			
			for (int i = 0; i < count_index; i++) {
				if (change_settings.equals("n")) {
					String temp;
					System.out.println("SET PARAMETERS FOR INDEX NUMBER " + i + ".");
					System.out.println("\nMemory:");
					response = new BufferedReader(new InputStreamReader(System.in));
					temp = response.readLine();
					mem = Long.parseLong(temp);
					System.out.println("Delfinal (true/false):");
					response = new BufferedReader(new InputStreamReader(System.in));
					temp = response.readLine();
					delfinal = Boolean.parseBoolean(temp);
					System.out.println("Doclens (true/false):");
					response = new BufferedReader(new InputStreamReader(System.in));
					temp = response.readLine();
					doclens = Boolean.parseBoolean(temp);
					System.out.println("Starting Index Number:");
					response = new BufferedReader(new InputStreamReader(System.in));
					temp = response.readLine();
					index_number = Long.parseLong(temp);
					System.out.println();
				}
				else if (change_settings.equals("y")) {
					System.out.println("Creating index file " + i + " ...");
					index_number = i;
				}
				else {
					System.out.println("INVALID INPUT. SKIPPING CREATING THE INDEX FILE(S).");
					break;
				}
				/*Given the name of the main parameter file responsible for generating index of the runtime format, converts the already parsed data and generates 
				the runtime format into the bibfiles directory. In case, the Javokapi package is unable to handle these functions, they can be manually called by executing 
				the shell command "sh X", where X is the appropriate function being used.*/
		        okapi.createIndex(db_param, mem, delfinal, doclens, index_number);			
			}
		}
		else {
			System.out.println("SKIPPING CREATING THE INDEX FILE(S).");
		}	
			System.out.println("=====================================");		
			System.out.println("Build Index Program Complete.\n");
	}

	
	public static void main(String[] args) throws Exception{
	 	build_index s = new build_index();
 	}

} 
