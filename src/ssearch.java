import java.io.*;
import java.util.LinkedList;

public class ssearch
{
	public static relex okapi_interface;
	// DAMON: ADDED JULY 14, 2007
	// THIS IS THE STARTING INDEX FOR EXPANDED TERMS OF EACH OF THE TOPICS IN ORDER.
	// THIS HELPS DETERMINE WHERE IN THE LL1 LINKED LIST OF SQUERY THE EXPANDED
	// TERMS ARE LOCATED.
	private static int[] expandedTermsIndecies;
	public static void main (String[]args) throws Exception
  	{
    	okapi_interface = new relex();
    	okapi_interface.javainit();
    	Infokit ink = new Infokit();
		
		// DAMON: ADDED JULY 4, 2007
		// TO FURTHER ENHANCE THE WORKING OF PROGRAM. MANY OF THE HARDCODED INFORMATIONS
		// WILL BECOME COMMAND LINE ARGUMENTS SO ELIMINATE THE NEED TO RECOMPILE THE PROGRAM.

		if (args.length != 9)
		{
			System.out.println("wrong number of arguments passed.");
			System.out.println("java ssearch <database name> <main index> <passage id index> <topics file> <feedback weight> <# output passages> <# top passages> <expanded terms weight> <expanded terms file>");
			System.exit(0);
		}
		String databaseName = args[0];
		String passageIndex = args[1];
		String passageIDIndex = args[2];
		String topicsFile = args[3];
    	float feedbackWeight = new Float(args[4]).floatValue();
    	int outputNumber = Integer.parseInt(args[5]);
		int numberOfTopPassages = Integer.parseInt(args[6]);
		// DAMON: ADDED JULY 4, 2007
		float expandedTermsWeight = Float.parseFloat(args[7]);
		String expandedTermsIndexFile = args[8];
		// DAMON: JULY 3, 2007
		// SOME BASIC ERROR CHECKING IN CASE ANY SIMPLE MISTAKE IS MADE
		if (feedbackWeight < 0)
		{
			System.out.println("feedbackWeight cannot be zero.");
			System.exit(0);
		}
		if (outputNumber < 0)
		{
			System.out.println("outputNumber cannot be zero.");
			System.exit(0);
		}
		if (numberOfTopPassages < 0)
		{
			System.out.println("numberOfTopPassages cannot be zero.");
			System.exit(0);
		}
		if (outputNumber < numberOfTopPassages)
		{
			System.out.println("outputNumber cannot be less than numberOfTopPassages. If you want only " + outputNumber);
			System.out.println("to be retreived, then there are " + outputNumber + "top passages. Cannot ask for ");
			System.out.println(numberOfTopPassages + "top passages.");
			System.exit(0);
		}

		// DAMON: ADDED ON JUN 30, 2007
	 	// CHOOSES THE DATABASE FILE AND THE SPECIFIED INDEX THAT MUST BE USED
	 	// FOR PARSING AND FINDINF OF RESULTS. IF WRONG DATABASE OR INDEX NAME
	 	// IS PASSED, AT THE MOMENT, WE DO NOT HAVE A WAY OF CATCHING THE ERROR.
		okapi_interface.comm ("ch " + databaseName);
		okapi_interface.comm ("set attribute=" + passageIndex);

		// DAMON: ADDED ON JUN 30, 2007
	 	// THE PROGRAM CREATES MANY FILES FOR EACH OF THE TOPICS IT PROCESSES ALONG
		// WITH MANY OTHER FILES SUCH AS TOP PASSAGES. WE CREATE A DIRECTORY FOR 
	 	// THIS PURPOSE SO ALL THE GENERATED FILES ARE PLACED INSIDE IT.
	 	String outputDirectory = args[0] + "-" + args[1] + "-" + args[2] + "-" + args[3] + "-" + args[4] + "-" + args[5] + "-" + args[6] + "-" + args[7];
		outputDirectory = outputDirectory + "/";
		System.out.println(outputDirectory);
	 	boolean success = (new File(outputDirectory)).mkdir();
    	if (!success)
		{
      		System.out.println("Program cannot create a directory to output results.");
      		System.out.println("Please make sure there is the program has permission to do so.");
			System.exit(0);
    	}
		// DAMON: JULY 4, 2007
		// VARIOUS FILE WRITERS TO SAVE THE OUTPUT OF FEEDBACK AND TOP PASSAGES.
		BufferedWriter feedbackWriter = new BufferedWriter(new FileWriter(outputDirectory + "feedback-terms"));
		BufferedWriter topPassageWriter = new BufferedWriter(new FileWriter(outputDirectory + "top-passageIDs"));
		
		// DAMON: JULY 21, 2007
		// WE READ THE INFORMATION ABOUT THE STARTING INDEX FILES FROM A FILE
		BufferedReader expandedIndexFileReader = new BufferedReader (new FileReader (expandedTermsIndexFile));
		String line = null;
		
		LinkedList<Integer> temp = new LinkedList<Integer>();
		while((line = expandedIndexFileReader.readLine()) != null)
		{
			temp.add(Integer.parseInt(line.substring(line.indexOf(':') + 1).trim()));
		}		
		expandedTermsIndecies = new int[temp.size()];
		for (int i = 0; i < expandedTermsIndecies.length; i++)
			expandedTermsIndecies[i] = temp.get(i);
		
		BufferedReader input = new BufferedReader (new FileReader (topicsFile));
		BufferedWriter bwr = new BufferedWriter (new FileWriter (outputDirectory + "structured-query-of-" + topicsFile));
		// DAMON: ADDED JULY 14
		// POINTS TO THE CORRECT INDEX FOR EACH TOPIC
		int expandedTermsIndex = 0;

  		while ((line = input.readLine ()) != null)
		{
      		if (line.trim ().equals (""))
	   		{
	  			continue;
		   	}
      		String[]parts = line.split (">");
      		String num = parts[0].replaceAll ("<", "");
      		//remove 's and i.e and i.e.
      		String contents = parts[1].replaceAll ("'s", "").replaceAll ("i\\.e", " ").replaceAll ("i\\.e\\.", " ");
      		bwr.write ("<ID>" + new Integer (num) + "</ID>\n");
      		bwr.flush ();

			/*
       		 * using the orignal query to build a linkedlist of snip_seq
       		 * using the snip_seq,a linkedlist of snoppet which built before, to build a bssset linkedlist
       		 */
	      	squery squ = new squery (contents, okapi_interface, ink, passageIndex, passageIDIndex, feedbackWeight, outputNumber, numberOfTopPassages, topPassageWriter, feedbackWriter, expandedTermsIndecies[expandedTermsIndex], expandedTermsWeight);

      	 	System.out.print ("" + new Integer (num) + "::");
      	 	squ.fb_qoutput (new Integer (num).intValue (), outputDirectory + "doc" + num, outputDirectory + "txt" + num);

      		System.out.println ();
      		squ.outputq (new Integer (num).intValue (), bwr, null);
      		okapi_interface.comm ("delete all");
      		expandedTermsIndex++;
		}
  		bwr.close ();
		topPassageWriter.close();
		input.close();
		feedbackWriter.close();
		expandedIndexFileReader.close();
	}
}
