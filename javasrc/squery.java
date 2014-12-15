import java.io.*;
import java.util.*;

public class squery
{
	public String orig_query;
    public LinkedList snip_seq;
    public Hashtable ab_pairs;
    public String query_wo_ab;
    public LinkedList half_stop;
    public relex x;
    Infokit ifk;
    Hashtable termlist;
    Hashtable stemmed_termlist;
    LinkedList bsssetseq;
    Hashtable hty;   
    private Enumeration feedbackTerms;

	String passageIndex;
	String passageIDIndex;
    float feedbackWeight;
    int outputNumber;
	int numberOfTopPassages;

	BufferedWriter topPassageWriter;
	BufferedWriter feedbackWriter;
	// DAMON: ADDED JULY 14, 2007
	// TO ASSIGN A DIFFERENT WEIGHT TO EXPANDED TERMS
	int expandedTermsStartingIndex;
	float expandedTermsWeight;
    
    squery(String q, relex r, Infokit ik, String pi, String pii, float fw, int on, int notp, BufferedWriter tpw, BufferedWriter fbw, int etsi, float etw)
	{
        orig_query = q;
		x = r;
        ifk = ik;
		passageIndex = pi;
		passageIDIndex = pii;
        feedbackWeight = fw;
		outputNumber = on;
		numberOfTopPassages = notp;
		
		topPassageWriter = tpw;
		feedbackWriter = fbw;
		expandedTermsStartingIndex = etsi;
		expandedTermsWeight = etw;
		
        half_stop = new LinkedList();
        stemmed_termlist = new Hashtable();
		snip_seq=snippet_maker(orig_query, ik);
        for (int i = 0; i < half_stop.size(); i++)
		{        	
            String hsp = (String) (half_stop.get(i));
            snip_seq.add(new snippet(hsp, x));
        }
		//hubin
        System.out.println("SEQURYSEQURY squery() after build snip_seq. snip_seq = " + snip_seq.toString());
	}

    /*
     *  only called by main in ssearch class: "squ.outputq(new Integer(num).intValue(), bwr, null);"
     *  write the whole concept words into a file.
     */    
    public void outputq(int qryno,BufferedWriter opt,LinkedList ll2) throws Exception
	{
    	LinkedList ll1 = getsearchsets();	//a linkedlist of bssset objects
    	if(ll2!=null) ll1.addAll(ll2);      //because ll2 always is null, it is not accessable.
    
    	for(int i=0;i<ll1.size();i++)
		{
    		bssset bst = (bssset)(ll1.get(i));  
    		opt.write(bst.concept() +"\n");	//returns a string representing all words in one concept and the corresponding discount
    	 }     
     }
  
     /*
      * using the "snip_seq" to get a  a linkedlist of bssset objects which is built before the query when the original query is parsed 
      * call the getsearchsets() in snippet class, and in turn call call the getsearchsets() in phrase class  
      * whcih  return a linkedlist of bssset objects
      */
    public LinkedList getsearchsets()
	{
    	System.out.println("SQUERYSQUERYSQUERY getsearchsets() beginning snip_seq=" + snip_seq.toString());
        LinkedList ll1 = new LinkedList();

        if (bsssetseq != null)
		{
            ll1.addAll(bsssetseq);
            System.out.println("SQUERYSQUERYSQUERY GETSEARCHSETS: TEST WHETHER bsssetseq is null or not"); //always null  
            return ll1;
        }

        for (int i = 0; i < snip_seq.size(); i++)
		{            
            snippet spt = (snippet) (snip_seq.get(i));
            ll1.addAll(spt.getsearchsets());
        }
        System.out.println("SQUERYSQUERYSQUERY getsearchsets() end return ll1=" + ll1.toString());
        return ll1;
    }
     
    /*called by main, like this  "squ.fb_qoutput(new Integer(num).intValue(), "result/doc" + num, "result/txt" + num);"
     * 
     * using the snip_seq, a linkedlist of snoppet which built before, to do searching and retiurn the result
     *   (1) using the snip_seq retriver the linkedlist of bssset; -- getsearchsets() which return "ll1"
     *   (2) "ll1" is used to do first time searching;  -- qoutput(qryno, filename1, null)
     *   (3) using the returned first 10 passages to rebuild a new linkedlist of bssset -- feedback(docset, this.toString())
     *   (4) redo the second time searching by the new new linkedlist of bssset -- qoutput(qryno, filename2, ll2)
     */	
    public void fb_qoutput(int qryno, String filename1, String filename2) throws Exception
	{
		//return the first 10 passages which are found by original bsssets linkedlist
        LinkedList docset = qoutput(qryno, filename1, null);
        LinkedList ll2 = new LinkedList();

		//parse the first 10 passages which are found by original bsssets linkedlist to build a new term list
        termlist = feedback(docset, this.toString());
		feedbackWriter.write(new String("Topic #" + qryno));
		feedbackWriter.newLine();
		StringTokenizer tokens = new StringTokenizer(termlist.toString().replaceAll("\\{", "").replaceAll("\\}", ""), ",");
		while (tokens.hasMoreTokens())
		{
			feedbackWriter.write(tokens.nextToken());
			feedbackWriter.newLine();
			feedbackWriter.flush();
		}
		feedbackWriter.write("----------------------------------------");
		feedbackWriter.newLine();
		feedbackWriter.flush();

		Enumeration tms = termlist.keys();
        while (tms.hasMoreElements())
		{
        	String key = (String) (tms.nextElement());

			// in feedback, "termlist.put(selected, new Float(feedbackWeight));", the wei should always be feedbackWeight.
            Float wei = (Float) (termlist.get(key));
            LinkedList ll3 = new LinkedList();
            ll3.add(key);
            bssset bst = new bss(x).condition_search(ll3, null, wei.floatValue());
            ll2.add(bst);
        }
        qoutput(qryno, filename2, ll2);
    }

  /*
     * First called: "LinkedList docset = qoutput(qryno, filename1, null);" in "fb_qoutput"     
     *         (1) only use the original bssset linkedlist to perform the search
     *         (2) write the found first 1000 papers into result/doc#
     *         (3) return the first 10 papers, which is used to call feedback
     *  
     * Second called: "qoutput(qryno, filename2, ll2);" in "fb_qoutput"
     *         (1) only use the rebuild bssset linkedlist to perform the search
     *         (2) write the found first 1000 papers into result/txt#
     * 
     * Note: the first time result is written into "filename1" which is "/result/doc***". It is the result of the first time query 
     *       the second time result is written into "filename2" which is "/result/txt***". It is the result of the second time query 
     * 
     * For example: query1: <100>procedure or methods for how to open up a cell through a process called electroporation.
     * creat_searchset: sets1 in search_list [Set No:3,   Total:236810  ,   Weight:2.912,   Combi: procedure+]
     * creat_searchset: sets1 in search_list [Set No:5,   Total:813941  ,   Weight:1.535,   Combi: methods+]
     * creat_searchset: sets1 in search_list [Set No:7,   Total:111586  ,   Weight:3.692,   Combi: open+]
     * creat_searchset: sets1 in search_list [Set No:9,   Total:1055453  ,  Weight:1.209,   Combi: cell+]
     * creat_searchset: sets1 in search_list [Set No:13,  Total:3333551  ,  Weight:-0.975,  Combi: a+alpha+]
     * creat_searchset: sets1 in search_list [Set No:12,  Total:394457  ,   Weight:2.365,   Combi: process+]
     * creat_searchset: sets1 in search_list [Set No:15,  Total:53810  ,    Weight:4.435,   Combi: called+]
     * creat_searchset: sets1 in search_list [Set No:17,  Total:2637  ,     Weight:7.461,   Combi: electroporation+]
	 *
     * First called: "LinkedList docset = qoutput(qryno, filename1, null);" in "fb_qoutput"
     * 263:sbf.toString():     s=3 w=2.912 s=5 w=1.535 s=7 w=3.692 s=9 w=1.209 s=13 w=-0.975 s=12 w=2.365 s=15 w=4.435 s=17 w=7.461 
     * Second called: "qoutput(qryno, filename2, ll2);" in "fb_qoutput"
     * 3136:sbf.toString():    s=3 w=2.912 s=5 w=1.535 s=7 w=3.692 s=9 w=1.209 s=13 w=-0.975 s=12 w=2.365 s=15 w=4.435 s=17 w=7.461
	 *                         s=584 w=0.46111998 s=585 w=0.16884999 s=586 w=0.23936 s=587 w=0.46254998 s=588 w=0.43824 s=589 w=0.82071
	 *                         s=590 w=0.43835 s=591 w=0.41756 s=592 w=0.42823 s=593 w=0.42163 s=594 w=0.39512 s=595 w=0.10571
	 *                         s=596 w=0.56441003 s=597 w=0.58883 s=598 w=0.25739998 s=599 w=0.35464 s=600 w=0.44726 s=601 w=0.85899
	 *                         s=602 w=0.41656998 s=603 w=0.13299 
     */
    public LinkedList qoutput(int qryno, String filename, LinkedList ll2) throws Exception
	{
		//ll1 is snip_seq  --hubin
        LinkedList ll1 = getsearchsets();
        float mainweight = 0.0f;
        for (int i = 0; i < ll1.size(); i++)
		{
            bssset bst = (bssset) (ll1.get(i));
            mainweight = mainweight + bst.weight;
        }
        
        float sub_weight = 0.0f;    
        if (ll2 != null)
		{
            for (int i = 0; i < ll2.size(); i++)
			{
                bssset bst = (bssset) (ll2.get(i));
                sub_weight = sub_weight + bst.weight;
            }
        }
        
        float scale = 1.0f;    //it is not used in fact, becaause "any number times 1.0" dose not change anything.
        
        LinkedList htb = new LinkedList();
        StringBuffer sbf = new StringBuffer();

        for (int i = 0; i < ll1.size(); i++)
		{
            bssset bst = (bssset) (ll1.get(i));
            if (i >= expandedTermsStartingIndex)
            {
	            Float t = new Float(bst.weight) * expandedTermsWeight;
            	sbf.append("s=" + new Integer(bst.setno) + " w=" + t + " ");
        	}
            else
            	sbf.append("s=" + new Integer(bst.setno) + " w=" + new Float(bst.weight) + " ");
        }
        
		//first time call, ll2 is null, not go inside; second time append the terms selected by feedback 
        if (ll2 != null)
		{
            for (int i = 0; i < ll2.size(); i++)
			{
                bssset bst = (bssset) (ll2.get(i));
                sbf.append("s=" + new Integer(bst.setno) + " w=" + new Float(bst.weight * scale) + " ");
            }
        }

        //the default value of k1 is 1.2 and bm25_b is 0.75, why not use default value?    --hubin
        String	result = x.comm("f " + sbf.toString() + " k1=1.4 bm25_b=0.55 op=bm25");         
        System.out.println("SQUERYSQUERYSQUERY \"qoutput\" sbf.toString():     " + sbf.toString());
        System.out.println("RECENT>>> " + result);
        String[] parsed = result.split(" maxwt=");
        String[] parsed2 = parsed[0].split(" np="); //number of papers
        int totaldoc = new Integer(parsed2[1]).intValue(); 
        StringBuffer final_result_doc = new StringBuffer();

		int topPassageCount = java.lang.Math.min(java.lang.Math.min(totaldoc, outputNumber), numberOfTopPassages);
		topPassageWriter.write("");
		topPassageWriter.newLine();
		topPassageWriter.flush();
        for (int i = 0; i < java.lang.Math.min(totaldoc, outputNumber); i++)
		{
            result = x.comm("s f=197 n=1");
            StringTokenizer pair1 = new StringTokenizer(result);
            String docnum = pair1.nextToken();
            Float weig = new Float(pair1.nextToken());
            final_result_doc.append(new Integer(qryno) + " Q0 " + docnum + " " + new Integer(i + 1) + " " + weig + " " + "<tag>\n");
            if (i < 10)
		{
				// hold the first 10 passages
                htb.add(docnum);
            }

			if (i < topPassageCount)
			{
				topPassageWriter.write(docnum);
				topPassageWriter.newLine();
				topPassageWriter.flush();
			}
        }
        BufferedWriter output = new BufferedWriter(new FileWriter(filename));
        output.write(final_result_doc.toString());
        output.close();
		
        return htb;
    }
    
	/* this method make tokens by / ' + ( ) . and whitespace first. Then do snippet for each word 
	 * For example, the query is: "a b a gene c d e genes f g"
	 * (1) get "a b a", remove redundant "a" ----> "a b"; build snippet for "a b"
	 * (2) add gene into linkedlist half_stop
	 * (3) get "c d e", remove redundant ---> "c d e"; build snippet for "c d e"
	 * (4) check "genes" is already in half_stop or not. If not, add it into linkedlist half_stop
	 * (5) get "f g", remove redundant ---> "f g"; build snippet for "f g"
	 * Note: the linkedlist half_stop contains all occurance half_stop_words, it will
	 * be build snippet one by one and added into the end of the final  snip_seq at about line 42.
	 */
	public LinkedList snippet_maker(String query, Infokit inf)
	{
        LinkedList ll = new LinkedList();
        String previous_snip = null;
        //withdraw the words form original query,  hubin
        StringTokenizer sto = new StringTokenizer(query, " /'+().");

        while (sto.hasMoreTokens())
		{
            String next = sto.nextToken().trim();
            String lnext = next.toLowerCase();
            Object hf_stop = inf.half_stop.get(lnext);

            System.out.println("********** 0:   \n"  +  next);
            if (previous_snip == null)
			{   //the first word
                if (hf_stop == null)
				{   //current word is not a half_word
                    previous_snip = next;
                    System.out.println("********** 1:   \n"  + previous_snip);
                }
				else
				{   //current word is a half_word                 
                    if (!half_stop.contains(lnext))
					{       
                        half_stop.add(lnext);
						//add current half_word into linkedlist "half_stop" if it is not in it.
                        System.out.println("********** 2:   \n"  + lnext);
                    }
                }
            }
			else
			{   //the first non half_word in query
                if (hf_stop == null)
				{   //current word is not a half_word
                    previous_snip = previous_snip + " " + next;
                    System.out.println("********** 3:   \n"  + previous_snip);
                }
				else
				{   //current word is a half_word   
                    if (!half_stop.contains(lnext))
					{
                        half_stop.add(lnext);
						//add current half_word into linkedlist "half_stop" if it is not in it.
                        System.out.println("********** 4:   \n"  + lnext);                            
                    }
					// build the snippets for those non_half_words bfore current hal_word
                    snippet sn1 = new snippet(redundant_clear(previous_snip), x);
                    System.out.println("********* 5:   \n" + sn1.toString());
                    ll.add(sn1);
                    previous_snip = null;
                }
            }
		}
        
        if (previous_snip != null)
		{
			// if last part of query is used to build snippet and added into linkedlist
        	//it put query string into snippet, it do another StringTokenizer to put every words
			//in a string array and call phrasize method to parse those words
            snippet sn2 = new snippet(redundant_clear(previous_snip), x);
            System.out.println("********* 7:   \n" + sn2.toString());            
            ll.add(sn2);
        }
        return ll;
    }

    public static String redundant_clear(String pre_snip)
	{
        StringTokenizer sto = new StringTokenizer(pre_snip, ",. ");
        LinkedList ll1 = new LinkedList();
        LinkedList ll2 = new LinkedList();
        LinkedList ll3 = new LinkedList();
        String result = "";

        while (sto.hasMoreTokens())
		{
            String next = sto.nextToken();
            ll1.add(next);
        }
        for (int i = ll1.size() - 1; i >= 0; i--)
		{
            String str1 = (String) (ll1.get(i));
            String str2 = str1.toLowerCase();

            if (!ll2.contains(str2))
			{
                ll2.add(str2);
                ll3.addFirst(str1);
            }
        }
        for (int j = 0; j < ll3.size(); j++)
		{
            result = result + (String) (ll3.get(j)) + " ";
        }
        return result;
    }

    public String toString()
	{
        return orig_query;
    }

    /*
     * only called in method "fb_qoutput":  termlist = feedback(docset, this.toString()); 
     * It uses the first 10 passages which are return by original bssset searching to perform the feedback
     * 
     * ll: the linkedlist which hold the number of the first 10 passages which are found by qoutput first time called.
     * qry: it is "orig_query" in fact, because this method is called as  "termlist = feedback(docset, this.toString());" 
     * 
     * (1)take the first 10 paper
     * (2)super parse whole terms and only keep those term which has different stem;
     * (3)for each term, calclulate its (3-1) w = Math.log((pdf + 0.5) / (p - pdf + 0.5) * (n - df - p + pdf + 0.5) / (df - pdf + 0.5));
     *                                (3-2) tsv = w * pcf;
                                      (3-3) tsvx = w * pdf  ;
     * (4)only select the term (4-1) in original query; OR
     *                         (4-2) tsv > 2; OR
     *                         (4-3) tsvx > 2; 
     * (5)remove "" and "a" form those selected terms in case.      
     * (6)assign the "feedbackWeight" to each selected term          
     */
    public Hashtable feedback(LinkedList ll, String qry)
	{
        int num = ll.size();    
        Hashtable termlist = new Hashtable();
        LinkedList resultl = new LinkedList();
        Hashtable dict = new Hashtable();
        Hashtable htx = new Hashtable();
        hty = new Hashtable();

        try
		{
            LinkedList terms = new LinkedList();
            LinkedList termsx = new LinkedList();

			while (ll.size() != 0)
			{
                String docid = (String) (ll.removeFirst());
				// DAMON: ADDED JULY 2, 2007
				// TO RETREIVE THE PASSAGES BY PASSGE ID, WE HAVE BUILT A SPECIAL INDEX. WE 
				// SWITCH TO THAT INDEX TEMPORARILY TO RETRIEVE PASSAGES. ONCE PASSAGES ARE
				// RETRIEVED WE GO BACK TO OUR MAIN INDEX.
				x.comm("set a=" + passageIDIndex);
                x.comm("f t=" + docid);
                String resu = x.comm("s");   
				
                StringBuffer sb = new StringBuffer();
                BufferedReader br = new BufferedReader(new StringReader(resu));
                String line = null;

                while ((line = br.readLine()) != null)
				{
					if (line.startsWith(" 2:"))
					{
                		sb.append(" " + line.replaceFirst(" 2:", ""));
                    }
				}
				x.comm("set a=" + passageIndex); 
                resu = x.comm("sp t=" + sb.toString().trim());
                String[] unit = resu.replaceAll("^[0-9]+", "").split("t=");
                StringBuffer sbu = new StringBuffer();

                for (int i = 0; i < unit.length; i++)
				{
                    String check = unit[i];
                    if (check.matches(".*c=H.*"))
					{
                        continue;
                    }
                    String t2 = check.replaceAll(" c=[A-Z] s=.*", "");
                    String t1 = check.replaceAll(".* c=[A-Z] s=", "");
                    sbu.append(t2 + "#");
                    if (!dict.containsKey(t2))
					{
                        dict.put(t2, t1);
                    }
                }
                StringTokenizer stro = new StringTokenizer(sbu.toString(), "#");
                Hashtable temp = new Hashtable();

                //this while loop calculate the occurrence time of parsed term
                while (stro.hasMoreTokens())
				{
                    String theone = stro.nextToken();
                    if (temp.containsKey(theone))
					{
                        Integer coun = (Integer) (temp.get(theone));
                        temp.put(theone, new Integer(coun.intValue() + 1));
                    }
					else
					{
                        temp.put(theone, new Integer(1));
                    }
                }
                Enumeration enu = temp.keys();
                //build htx and hty, there 10 papers!
                //htx records (1) the parsed term (2) the total number of the occurence time in 10 papers
                //hty records (1) the parsed term (2) the number of the paper which has this parsed term
                while (enu.hasMoreElements())
				{
                    String ak = (String) (enu.nextElement());
                    if (htx.containsKey(ak))
					{
                        Integer coun = (Integer) (temp.get(ak));
                        Integer precoun = (Integer) (htx.get(ak));
                        htx.put(ak, new Integer(coun.intValue() + precoun.intValue()));
                    }
					else
					{
                        Integer coun = (Integer) (temp.get(ak));	
                        htx.put(ak, coun);
                    }
                    if (hty.containsKey(ak))
					{
                        Integer precoun = (Integer) (hty.get(ak));
                        hty.put(ak, new Integer(1 + precoun.intValue()));     
                    }
					else
					{
                        hty.put(ak, new Integer(1));
                    }
                }
            }
			//hold the parsed terms
            Enumeration enum1 = htx.keys();

            while (enum1.hasMoreElements())
			{
                String key = (String) (enum1.nextElement());
                String ret = x.comm("f t=" + key);
                System.out.println("SQUERYSQUERYSQUERY \"feedback\"  key :            " + key);
                System.out.println("SQUERYSQUERYSQUERY \"feedback\"  ret :            " + ret); 
							
                ret = ret.replaceAll(".*np=", "").replaceAll(" t=.*", ""); //the number of np
                double df = new Double(ret).doubleValue();
                String pdfs = ((Integer) (hty.get(key))).toString();
                double pdf = new Double(pdfs).doubleValue();   //the number of papers which has this parsed term
                double n = 4560000.00d;                      //nearly the number of total records
                double p = new Double(num).doubleValue();   //should be 10 here
                String pcfs = ((Integer) (htx.get(key))).toString();   //should be occurrence time of the parsed term in whole 10 paper
                double pcf = new Double(pcfs).doubleValue();
                double 
                        w = Math.log(
                        (pdf + 0.5) / (p - pdf + 0.5) * (n - df - p + pdf + 0.5)
                        / (df - pdf + 0.5));
                double tsv = w * pcf;
                double tsvx = w * pdf;
                System.out.println("SQUERYSQUERYSQUERY \"feedback\":  df = " + df + " ;  pdf = " + pdf + " ; n = " + n 
                		                                        + " ; p = " + p + " ;  pcf= " +pcf);                              //hubin
                System.out.println("SQUERYSQUERYSQUERY \"feedback\":  w = " + w + " ; tsv = " + tsv + " ; tsvx = " + tsvx );      //hubin

                terms.add(new pair(key, new Double(tsv)));  //the parsed trem and its tsv number are added into the linkedlist terms
                termsx.add(new pair(key, new Double(tsvx)));  //the parsed trem and its tsvx number are added into the linkedlist termsx

            }
            Collections.sort(terms, new myComp());
            Collections.sort(termsx, new myComp());
            for (int i = 0; i < 20; i++)
			{
                pair thispair = (pair) (terms.remove(i));
				//the pared term in terms which is build by "terms.add(new pair(key, new Double(tsv)));"
                String term = (String) (thispair.key);
                pair thatpair = (pair) (termsx.remove(i));
                String termx = (String) (thatpair.key);
                int c_term = ((Integer) (hty.get(thispair.key))).intValue();    //the value in hty  
                int c_termx = ((Integer) (hty.get(thatpair.key))).intValue();    //the value in hty   
                boolean 
                        potent1 = c_term >= 2                 //why take 2?
                        || (qry.toLowerCase().matches(                      
                                ".*" + term.toLowerCase().trim() + ".*"));   //this term in original query
                boolean 
                        potent2 = c_termx >= 2               //why take 2?
                        || (qry.toLowerCase().matches(                       
                                ".*" + termx.toLowerCase().trim() + ".*"));  //this term in original query

                if ((!resultl.contains(term)) && potent1) {
                    resultl.add(term);                          //result take all satisfied parsed term
                }
                if ((!resultl.contains(termx)) && potent2) {
                    resultl.add(termx);                          //result take all satisfied parsed term
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        int count = 0;

        while (resultl.size() != 0)
		{
            count++;
            String te = (String) (resultl.remove());   //each selected parsed term
            String selected = (String) (dict.get(te));
		
            if ((!selected.trim().equals("")) && (!selected.toLowerCase().trim().equals("a")))
			{
	            if (selected.trim().matches("[a-zA-Z0-9]+"))
				{
                	termlist.put(selected, new Float(feedbackWeight));
                }
            }
        }
        System.out.println("SQUERYSQUERYSQUERY \"feedback\"  termlist :   " + termlist);     
        return termlist;
    }
}

