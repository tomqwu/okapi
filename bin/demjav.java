import java.util.*;
import java.io.*;
import java.lang.*;
import java.util.regex.*;

//AUTHOUR: RAYMOND NUQUI
//DATE: 1 May 2007

//THIS METHOD SHOWS THAT THE OUTPUTS OF JAVOKAPI ARE THE SAME AS THOSE OF OKAPI.  

public class demjav{
	
	public demjav() throws Exception{
	        String out = "Welcome to the Okapi Information Retrieval Engine.\n";
	        javokapi okapi = new javokapi();

		System.out.println(okapi.chooseDB("2009trec")); //  ch   med.sample
		System.out.println(okapi.superParse("", "curable composition")); //sp   t=heart attack thrombosis cardiac arrest 
		System.out.println(okapi.find("","","","curabl")); // f   t=heart 
		System.out.println(okapi.weight("2","50","0","0","4","5")); //w   fn=2   r=0   bigr=0   rload=4   bigrload=5   n=50 
		System.out.println(okapi.find("","","","composit"));//f   t=attack
		System.out.println(okapi.weight("2","7","0","0","4","5")); //w   fn=2   r=0   bigr=0   rload=4   bigrload=5   n=7  
		
	}

	
	public static void main(String[] args) throws Exception{
	 	demjav s = new demjav();
 	}

} 
