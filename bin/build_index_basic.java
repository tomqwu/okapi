import java.util.*;
import java.io.*;
import java.lang.*;
import java.util.regex.*;

//AUTHOUR: RAYMOND NUQUI
//DATE: 1 May 2007

//THIS IS A SIMPLE DEMONSTRATION OF THE METHODS IN JAVOKAPI.

public class build_index_basic{
	
	public build_index_basic() throws Exception{
        String out = "Welcome to the Okapi Information Retrieval Engine.\n";
        javokapi okapi = new javokapi();  //CREATES NEW INSTANCE OF JAVOKAPI


//------------------------------------------------------------------------------	
        System.out.println(okapi.exchangeFormat("gen_parser","trec04", "gen_exch"));
        /*Given the name of main parameter file responsible for generating runtime format, converts the already parsed data and generates the runtime format into the bibfiles directory. In case, the Javokapi package is unable to handle these functions, they can be manually called by executing the shell command "sh X", where X is the appropriate function being used.
        */
        okapi.runtimeFormat("2004gendoc", "gen_exch");
        /*Given the name of the main parameter file responsible for generating index of the runtime format, converts the already parsed data and generates the runtime format into the bibfiles directory. In case, the Javokapi package is unable to handle these functions, they can be manually called by executing the shell command "sh X", where X is the appropriate function being used.
        */
        okapi.createIndex("2004gendoc",100, true, true, 0);

//------------------------------------------------------------------------------
		
	}

	
	public static void main(String[] args) throws Exception{
	 	build_index_basic s = new build_index_basic();
 	}

} 
