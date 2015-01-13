import java.io.*;
import java.util.*;

public class expandedTermIndexFinder
{
	public static relex okapi_interface;
	public static void main(String[] args) throws Exception
	{
		String inFilename = args[0];
		String outFilename = "expansion-index-of-" + inFilename;
		
		okapi_interface = new relex();
    	okapi_interface.javainit();
    	okapi_interface.comm("ch " + args[1]);
    	okapi_interface.comm("set a=" + args[2]);

		BufferedReader reader = new BufferedReader(new FileReader(inFilename));
		BufferedWriter writer = new BufferedWriter(new FileWriter(new File(outFilename)));
		String line = "";
		String counter = "";
		while ((line = reader.readLine()) != null)
		{
			line = line.replaceFirst("<", "");
			counter = line.substring(0, line.indexOf('>'));
			String output = "Topic #" + counter + " : ";
			line = line.substring(line.indexOf('>') + 1, line.length());
			String okapiOutput = okapi_interface.comm("p t=" + line);
			StringTokenizer tokens = new StringTokenizer(okapiOutput);
			String[] terms = new String[Integer.parseInt(tokens.nextToken())];
			for (int i = 0; i < terms.length; i++)
			{
				String s = tokens.nextToken();
				terms[i] = s.substring(s.indexOf('=') + 1);
			}
			output = output + countNonDuplicateTerms(terms);
			writer.write(output);
       		writer.newLine();
       		writer.flush();
		}
		reader.close();
		writer.close();
	}
	
	private static int countNonDuplicateTerms(String[] x)
	{
		int count = 0;
		for (int i = 0; i < x.length; i++)
		{
			boolean duplicateFound = false;
			String str = x[i];
			for (int j = 0; j < i; j++)
				if (x[j].equals(str))
				{
					duplicateFound = true;
					break;
				}
			if (!duplicateFound)
				count++;
		}
		return count;
	}
}
