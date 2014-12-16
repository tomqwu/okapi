
import java.util.*;

public class phrase {
    public static relex x;
    public LinkedList term_seq;       //the linkedlist contains all words in original query.
    public LinkedList subterm_seq;   //the linkedlist contains the subterms object which are created after  subtermize() "breaking" for each word
    public LinkedList subterm_combination1;  //the linkedlist contains the subterms combined with option
  //  public String[] tokens;
    public bssset adj_set;
    public LinkedList search_list;   //the linkedlist hold the bssset which will used for query and is built at end of parse (at the end of constructor 
    public bss mybss;
  //  public Infokit inf;

    
    
    phrase(String text_t, relex rx) {
     //   System.out.println("Construting Phrase: " + text_t);
        x = rx;
        mybss = new bss(x);
        term_seq = new LinkedList();
	
        search_list = new LinkedList();
        StringTokenizer sto = new StringTokenizer(text_t);
		// Hu Bin: should be no useful, it called by phrasize method in snippet,
		//only one word be passed in this method

        while (sto.hasMoreTokens()) {
            term_seq.add(sto.nextToken());
        }

        BigDigitbreak();                  //change term_seq by deal with I II III
        System.out.println("PHRASEPHRASE after BigDigitbreak  term_seq:  " + term_seq);
		//add by hubin for print  
        subtermize();                    // three breakings
        System.out.println("PHRASEPHRASE after subtermized  subterm_seq:  " + subterm_seq);
		//add by hubin for print  
        /*
         *   variant (option) will be combined at "combine" method
         */    
        subterm_seq = new Infokit().variant_creator(subterm_seq);       //add the option word  
        print_subterm_seq();                     

        subterm_combination1 = combine(true, false);             //combine the option words together. need waiting test result
        System.out.println("PHRASEPHRASE constructor,  check method \"combine\"");                                      //add by hubin
        for(int i = 0; i < subterm_combination1.size(); i++)                                                                  //add by hubin
        	System.out.println("PHRASEPHRASE constructor,  subterm_combination1 [" + i + "] is:  "  + subterm_combination1.get(i));  //add by hubin
    //    System.out.println("Combine1:" + print_combination(subterm_combination1));

        if (subterm_combination1 != null && subterm_combination1.size() > 0) {
	
            LinkedList sets1 = creat_searchset(subterm_combination1);   //get a linkedlist of beeset
            search_list.addAll(sets1);
            System.out.println("PHRASEPHRASE constructor, after creat_searchset:  sets1 in search_list " +  sets1.toString());                                      //add by hubin
        }		
    }
    
	/*
    * return a linkedlist which contains bssset objects (which is built at the end of constructor of this class 
    * when the original is parsed.)
    */
    public LinkedList getsearchsets() {
        LinkedList ll1 = new LinkedList();

        ll1.addAll(search_list);
        return ll1;
    }
    

    
    /*
     * using the combin() result "subterm_combination1" to build a linkedlist of bssset "search_list"
     */
    public LinkedList creat_searchset(LinkedList combination) {
        LinkedList sets = new LinkedList();
    //    String comb1 = print_combination(combination);                  //deleted by hubin

   //     System.out.println("Creating search set for " + comb1);        //deleted by hubin
        // if(comb1.matches(".*[a-zA-Z0-9]+ [a-zA-Z0-9]+.*")){
        adj_set = mybss.adjsearch(combination);         // using "subterm_combination1" return a bssset
        sets.add(adj_set);
  //      System.out.println("adj set:" + new Integer(adj_set.setno) + " " + new Float(adj_set.weight));           //deleted by hubin
        return sets;

    }

   public String print_combination(LinkedList comb1) {
        StringBuffer sbf = new StringBuffer();

        for (int i = 0; i < comb1.size(); i++) {
            String str1 = (String) (comb1.get(i));

            sbf.append(str1 + "+");
        }
        return sbf.toString().trim();
    }
   
   

    public void print_subterm_seq() {
        for (int i = 0; i < subterm_seq.size(); i++) {
            subterm  stm = (subterm) (subterm_seq.get(i));
            System.out.println("PHRASEPHRASE  print_subterm_seq():  after variant_creator, subterm_seq[" + i + "]=" + stm.toString());
        }
    }

   /*
    * If "III", "II" or "I" is at the end of word, separate them from the word as a new term 
    */
   public void BigDigitbreak() {
        LinkedList new_term_seq = new LinkedList();
        boolean changed = false;

        for (int i = 0; i < term_seq.size(); i++) {
            String curr_term = (String) (term_seq.get(i));

            if (curr_term.matches(".+[^ ](III)")) { 
                curr_term = curr_term.replaceAll("III$", "");
                new_term_seq.add(curr_term);
                new_term_seq.add("III");
                changed = true;
            } else if (curr_term.matches(".+[^ ](II)")) {
                curr_term = curr_term.replaceAll("II$", "");
                new_term_seq.add(curr_term);
                new_term_seq.add("II");
                changed = true;

            } else if (curr_term.matches(".+[^ ]I")) {
                curr_term = curr_term.replaceAll("I$", "");
                new_term_seq.add(curr_term);
                new_term_seq.add("I");
                changed = true;

            }

        }
        if (changed) {
            term_seq = new_term_seq;
        }
 
    }

    /*
     * called as:  subterm_combination1 = combine(true, false);
     * 
     * (1) remove the stop word by using super parse in okapi, (remove the word with "C=H" from the return result).
     * (2) craeate a "subterm_combination1" which includs kinds of monograms.
     * 
	 * For example:
	 * <147>Mutations of alpha7 nAChR gene and its/their biological impact in alcoholism.
	 *
     * 211631:PHRASEPHRASE  print_subterm_seq():  after variant_creator, SUBTERM_REP: SUBTERMSUBTERM object:  text = of;;;;    the option words = of+  //---stop word. removed
     * 211648:PHRASEPHRASE  print_subterm_seq():  after variant_creator, SUBTERM_REP: SUBTERMSUBTERM object:  text = alpha;;;;    the option words = alpha+a+
     * 211649:PHRASEPHRASE  print_subterm_seq():  after variant_creator, SUBTERM_REP: SUBTERMSUBTERM object:  text = 7;;;;    the option words = 7+
     * 211701:PHRASEPHRASE constructor,  subterm_combination1 [0] is:  alpha7
     * 211702:PHRASEPHRASE constructor,  subterm_combination1 [1] is:  alpha 7
     * 211703:PHRASEPHRASE constructor,  subterm_combination1 [2] is:  a7
     * 211704:PHRASEPHRASE constructor,  subterm_combination1 [3] is:  a 7
     * 211748:PHRASEPHRASE  print_subterm_seq():  after variant_creator, SUBTERM_REP: SUBTERMSUBTERM object:  text = n;;;;    the option words = n+
     * 211749:PHRASEPHRASE  print_subterm_seq():  after variant_creator, SUBTERM_REP: SUBTERMSUBTERM object:  text = AC;;;;    the option words = AC+
     * 211750:PHRASEPHRASE  print_subterm_seq():  after variant_creator, SUBTERM_REP: SUBTERMSUBTERM object:  text = h;;;;    the option words = h+
     * 211751:PHRASEPHRASE  print_subterm_seq():  after variant_creator, SUBTERM_REP: SUBTERMSUBTERM object:  text = R;;;;    the option words = R+Absent+receptor+
     * 212006:PHRASEPHRASE constructor,  subterm_combination1 [0] is:  nAChR
     * 212007:PHRASEPHRASE constructor,  subterm_combination1 [1] is:  nACh R
     * 212008:PHRASEPHRASE constructor,  subterm_combination1 [2] is:  nAC hR
     * 212009:PHRASEPHRASE constructor,  subterm_combination1 [3] is:  n AChR
     * 212010:PHRASEPHRASE constructor,  subterm_combination1 [4] is:  n ACh R
     * 212011:PHRASEPHRASE constructor,  subterm_combination1 [5] is:  nACh receptor
     * 212012:PHRASEPHRASE constructor,  subterm_combination1 [6] is:  n ACh receptor
     */     
   
    public LinkedList combine(boolean need_vali, boolean allowdel) {
        int len = subterm_seq.size();
        LinkedList cll = new LinkedList();

      	System.out.println("PHRASEPHRASE combine()  beginning subterm_seq = " + subterm_seq.toString());
        
        //if len==1, added the validate into cll
        if (len == 1) {                             
            subterm sbt = (subterm) (subterm_seq.get(0));
            for (int m = 0; m < sbt.size(); m++) {
                String substi = (String) (sbt.get(m));    //call option.get(i). Get option word (string) one by one
                if (!allowdel) {                     //allowdel always flase, it always need go inside.
                    if (need_vali) {                 //need_vali always true, it  go inside.
                    	System.out.println("PHRASEPHRASE combine()   if (len == 1)  substi = " + substi);
                    	System.out.println("BSSBSSBSS validate() is called by combine()  if (len == 1)");
                        if (mybss.validate(substi)) { 
                            cll.add(substi);
                        }
                    } else {                    //need_vali always true, it can not accessable.  
                        cll.add(substi);
                    }
                }
            }
            return cll;
        }

        int k = 0;
        int w = 1 << 31;                            //w always is -2147483648
        for (int i = 0; i < len - 1; i++) {
            k = k << 1;
            int s = 1;
            k = s | k;
        }
        System.out.println("PHRASEPHRASE combine : len = " + len);

        LinkedList option_chain = construct_option_chain();
        
        for (int d = 0; d < option_chain.size(); d++) {
            opchain opc = (opchain) (option_chain.get(d));
            LinkedList sbt = opc.chain;
            boolean isdel = opc.del;
            LinkedList curr_link = joiner(sbt, k, w);

            for (int r = 0; r < curr_link.size(); r++) {
                String link = (String) (curr_link.get(r));
                
                if ((!cll.contains(link.trim()))) {
                    if (allowdel) {                         //allowdel always flase, it always can not accessable.
                        if (isdel) {
                            cll = remove_redun(cll, link);
                            cll.add(link);
                        }
                    } else {                               //allowdel always flase, it always go inside. 
                        if (!isdel) {
                            cll = remove_redun(cll, link);
                            cll.add(link);
                        }						
                    }
                }
            }
        }
        if (option_chain.size() == 0) {
            LinkedList curr_link = joiner(new LinkedList(), k, w);

            for (int r = 0; r < curr_link.size(); r++) {
                String link = (String) (curr_link.get(r));
                if (!allowdel) {                                 //allowdel always flase, it always need go inside.
                    if (!cll.contains(link.trim())) {
                        cll = remove_redun(cll, link);
                        cll.add(link);
                    }
                }
            }
        }
    	System.out.println("PHRASEPHRASE combine() end return  cll = " +  cll.toString());
        return cll;	
    }

    public LinkedList remove_redun(LinkedList llx, String next) {
        LinkedList ll1 = new LinkedList();

        for (int i = 0; i < llx.size(); i++) {
            String existing = (String) (llx.get(i));
		
            if (!(existing.toLowerCase().matches(next.toLowerCase() + "[ ]+.*$")
                    || existing.toLowerCase().matches(
                            ".*[ ]" + next.toLowerCase() + "[ ].*")
                            || existing.toLowerCase().matches(
                                    ".*[ ]" + next.toLowerCase()))) {
                ll1.add(existing);	
            }
        }
        return ll1;
    }

    /* 
     * build a linkedlist ll1 by using the linkedlist of subterm ----> "subterm_seq" 
     * 
     * return ll1
     *         ll1 is a linkedlist of opchain
     *         opchain has (1) alinkedlist of op_pair (2) boolean
     *         op_pair has (1) subterm (2) string of one option word
     *         subterm has orignal term and its all option words
     */
    public LinkedList construct_option_chain() {
        LinkedList ll1 = new LinkedList();

        for (int i = 0; i < subterm_seq.size(); i++) {
            subterm curr_term = (subterm) (subterm_seq.get(i));

            if (curr_term.size() > 1) {   // curr_term.size() will return option.size(); If it > 1, there is at least one option word of original word
                if (ll1.size() == 0) {   // the first term which has at least one option word of original word
                    for (int j = 0; j < curr_term.size(); j++) {        
                        LinkedList ll5 = new LinkedList();
                        String strrep = (String) (curr_term.get(j));   //curr_term.get(j) get the option word one by one

                        ll5.add(new op_pair(curr_term, strrep));        //build op_pair (1)this subterm "curr_term" (2)one option word "strrep"
						
                        ll1.add(new opchain(ll5, strrep.equals("") ? true : false)); //strrep.equals("") should always be false
                       /*
                        *  Be careful!!! For ecample: if A has original string "a" with two options "b" and "c", then
                        *  (1) when j = 1,  ll5<1> is op_pair(A, a),  
                        *                   ll1 is opchain(ll5<1>, false)
                        *  (2) when j = 2,  ll5<2> is op_pair(A, a)<-->op_pair(A, b),   
                        *                   ll1 is opchain(ll5<1>, false)<-->opchain(ll5<2>, false)
                        *  It seems no reasonable.  This clause should be outside of this for block               --hubin
                        */
                    }                    
                } else {
                    LinkedList ll2 = new LinkedList();

                    for (int m = 0; m < ll1.size(); m++) {
                        opchain opc = (opchain) (ll1.get(m));
                        LinkedList ll3 = opc.chain;
                        boolean isdel = opc.del;

                        for (int n = 0; n < curr_term.size(); n++) {
                            LinkedList ll4 = new LinkedList();

                            ll4.addAll(ll3);
                            String strrep = (String) (curr_term.get(n));
                            boolean ifthisdel = strrep.equals("") || isdel;

                            ll4.add(new op_pair(curr_term, strrep));
                            ll2.add(new opchain(ll4, ifthisdel));
                        }
                    }
                    ll1 = ll2;

                }
            }
        }
        System.out.println("PHRASEPHRASE construct_option_chain return ll1" + ll1.toString());        
        return ll1;
    }

    public LinkedList joiner(LinkedList lterms, int k, int w) {
    	
    	System.out.println("PHRASEPHRASE joiner passed parameter: (1)lterms = " + lterms.toString());    
    	System.out.println("PHRASEPHRASE joiner passed parameter: (2) k = " + k + ";  w = " + w);    
        int upper = k;
 
        LinkedList lin_s = lterms;
        int shift_distance = 32 - subterm_seq.size() + 1;
        LinkedList ll = new LinkedList();

        // for(int i=0;i<s_term.size();i++){
        upper = k;
        while (upper >= 0) {
            int shift_upper = upper;

            shift_upper = shift_upper << shift_distance;
            String link = null;
            String previous_tok = null;
            String partial_link = null;
            LinkedList contain_chain = new LinkedList();
            LinkedList subt_list = new LinkedList();
            LinkedList bag = new LinkedList();

            for (int j = 0; j < subterm_seq.size(); j++) {
                subterm curr_term = (subterm) (subterm_seq.get(j));
         
                int term_number = curr_term.term_num;
      
                if (!bag.contains(new Integer(term_number))) {
                    bag.add(new Integer(term_number));
                }
                String tok;
                String subst = tlookup(lin_s, curr_term);

                if (subst != null) {
                    tok = subst;
                } else {
                    tok = curr_term.get(0);
                }
                if (previous_tok == null) {
                    previous_tok = tok;
                    link = tok;
                    if (!(tok == null) && (!tok.equals(""))) {
                        subt_list.add(curr_term);
                        partial_link = tok;
                    }
                    continue;
                } else {
                    int test_bit = shift_upper & w;

                    if (test_bit == w) {
                        // if((!ifstart)){
                        link = link.trim() + tok.trim();
   //                     System.out.println("LINKLINK1:" + link);

                        previous_tok = tok;
                        if (!(tok == null) && (!tok.equals(""))) {
                            subt_list.add(curr_term);
                            if (partial_link == null) {
                                partial_link = tok;
                            } else {
                                partial_link = partial_link + tok;

                            }
                        }

                    } else {
                        link = link.trim() + " " + tok.trim();
                        // System.out.println("LINKLINK2:"+link);
                        previous_tok = tok;
                        if (partial_link != null) {
                            contain_chain.add(
                                    new conta_part(subt_list, partial_link));
                            subt_list = new LinkedList();
                            if (!(tok == null) && (!tok.equals(""))) {
                                subt_list.add(curr_term);
                                partial_link = tok;
                            } else {
                                partial_link = null;
                            }
                        }
							
                    }
                    shift_upper = shift_upper << 1;

                }
            }
            if (partial_link != null) {
                contain_chain.add(new conta_part(subt_list, partial_link));
            }

            if (mybss.validate(link) && !ll.contains(link)) {	
                ll.add(link);
                addcontainer(contain_chain);
			
            }
            // if(!ll.contains(link))	ll.add(link);			
            upper--;
        }

        // }
        System.out.println("PHRASEPHRASE joiner return value ll = " + ll);  
        return ll;
    }

    public void addcontainer(LinkedList ll1) {
        for (int i = 0; i < ll1.size(); i++) {
            conta_part cpt = (conta_part) (ll1.get(i));
            LinkedList ll2 = cpt.c_chain;
            String c_str = cpt.c_str;

            for (int j = 0; j < ll2.size(); j++) {
                subterm sbt = (subterm) (ll2.get(j));

                sbt.add_super(c_str);
    //            System.out.println( "Add super string " + c_str + " to " + sbt.text());
            }

        }

    }

    public String tlookup(LinkedList lins, subterm sbt) {
        LinkedList lls = lins;

        for (int i = 0; i < lls.size(); i++) {
            subterm curr_t = ((op_pair) (lls.get(i))).first;

            if (sbt == curr_t) {
                return ((op_pair) (lls.get(i))).first_txt;

            }
        }
        return null;

    }

    public String printLinkList(LinkedList ll1) {
        String str2 = "";

        for (int i = 0; i < ll1.size(); i++) {
            String str1 = (String) (ll1.get(i));

            str2 = str2 + "+" + str1;
        }
        return str2;	
    }

    public LinkedList subterm_checkout(int j) {
        LinkedList ll1 = new LinkedList();

        for (int i = 0; i < subterm_seq.size(); i++) {
            subterm sbt = (subterm) (subterm_seq.get(i));

            if (sbt.term_num == j) {
                ll1.add(sbt);
            }
        }
        return ll1;

    }

    public LinkedList super_str(String term, int num) {
        for (int i = 0; i < subterm_seq.size(); i++) {
            subterm sbt = (subterm) (subterm_seq.get(i));

            if (sbt.term_num == num
                    && term.toLowerCase().equals(sbt.text().toLowerCase())) {
                return sbt.superstring;
            }
        }
        return null;

    }
    
   
    public String toString() {                    //hubin
    	String phraseobject= "\n     PHRASEPHRASE object  (1) term_seq             = " + term_seq.toString() + 
    	                     "\n     PHRASEPHRASE object  (2) subterm_seq          = " + subterm_seq.toString() + 
    	                     "\n     PHRASEPHRASE object  (3) subterm_combination1 = " + subterm_combination1.toString() +
    	                     "\n     PHRASEPHRASE object  (4) search_list          = " + search_list.toString();
        return phraseobject;
    }

    public void subtermize() {
        subterm_seq = new LinkedList();
        for (int i = 0; i < term_seq.size(); i++) {
            String term = (String) (term_seq.get(i));

     //       System.out.println("Term: " + term);
            LinkedList subterm_list = breaking(term, i);
                     
            for (int j = 0; j < subterm_list.size(); j++) {
                subterm s_term = (subterm) (subterm_list.get(j));            
                subterm_seq.add(s_term);
            }
            System.out.println("PHRASEPHRASE subtermized  subterm_seq:  " + subterm_seq);            //add by hubin for print           
        }
        return;
    }

    //a word separated by latin, and put each part into a linkedlist of object "subterm";
    // In each subterm, if the term is a latin, mark it as 1; otherwise, mark it as 0.
    public LinkedList latin_breaking(String term, int q) {
        Infokit inf = new Infokit();
        LinkedList llk = new LinkedList();
        LinkedList breaks = latin_breaking_sub(term, q);

        //be care, the loop1 will cause dead-loop!!!    --hubin
        loop1:                                                  //delete by hubin
        for (int i = 0; i < breaks.size(); i++) {
            String brk = (String) (breaks.get(i));
                               
            for (int j = 0; j < inf.latin_group.length; j++) {
                String latin = inf.latin_group[j];

                if (latin.equals(brk)) {
                    llk.add(new subterm(brk, 1, q));  
                    System.out.println("PHRASEPHRASEDDDDDDDDD can access loop! \"latin\"   "  + latin);    // test by hubin, answer yes
                    continue loop1;                               //deleted by hubin
                }
            }
            llk.add(new subterm(brk, 0, q));
				
        }
        return llk;
    }

    //para "q" is not useful in fact
    public LinkedList latin_breaking_sub(String term, int q) {
   //     System.out.println("Latin Break: " + term);
        String curr_term = term;
        Infokit inf = new Infokit();
        LinkedList llk = new LinkedList();
        String low_curr_term = curr_term.trim().toLowerCase();

        //this for loop is too complex. In fact, it just separate the word by latin, and put each part into a linkedlist and 
        //return linkedlist; If no latin in the word, just put the word in a empty linkedlist, and return it.   ----hubin
        for (int i = 0; i < inf.latin_group.length; i++) {
            String latin = inf.latin_group[i];
            int s_index = low_curr_term.indexOf(latin);

            if (s_index < 0) {
                continue;
            }
            //if latin at the begin of word, part1 = null; else part1 take the part before the latin. --  Hu Bin
            String part1 = s_index == 0 ? null : curr_term.substring(0, s_index); 
//          if latin at the end of word, part3 = null; else part3 take the part after the latin. --  Hu Bin 
            String part3 = s_index == curr_term.length() - latin.length()
                    ? null
                    : curr_term.substring(s_index + latin.length());
          
            LinkedList ll1 = part1 == null ? null : latin_breaking_sub(part1, q);
            LinkedList ll2 = new LinkedList();
            
            //if latin exist, add it to ll2
            ll2.add(latin);
            LinkedList ll3 = part3 == null ? null : latin_breaking_sub(part3, q);

            return  glue(glue(ll1, ll2), ll3);
        }
        //if no latin found, add this word into the linkedlist llk, and return it
        llk.add(curr_term);	  
        return llk;
    }

    public LinkedList glue(LinkedList ll1, LinkedList ll2) {
        if (ll1 == null) {
            return ll2;
        }
        if (ll2 == null) {
            return ll1;
        }
        for (int i = 0; i < ll2.size(); i++) {
            ll1.add(ll2.get(i));
        }
        return ll1;
    }

    //just break a word by digit. For example "01ab3c45#" will be broken into "01" "ab" "3" "c" "45" and all digit word will 
    // be marked as type 2.  hubin
    public LinkedList digit_breaking(subterm term, int q) {
 //       System.out.println("Digit Break: " + term.text());
        subterm curr_term = term;
        String term_txt = curr_term.text();
        LinkedList llist = new LinkedList();
        
        // it only can be latin which type is 1, because only latin break is called before it.
        // whatever this word type is, only if it already marked (non zero), it can not contain a digit inside. hubin
        if (curr_term.type > 0) {
            llist.add(curr_term);
            	//System.out.println("Check whether type can only be 1 which is latin.  \" type: \"" + curr_term.type);
            return llist;
        }
        
        StringBuffer sbf = null;
        boolean dig_status = false;

        term_txt = term_txt + "#";
   
        for (int i = 0; i < term_txt.length(); i++) {
            char s = term_txt.charAt(i);
            int k = (int) s;

            if (k >= 48 && k <= 57) {
                // ||(k==73&&i==term_txt.length()-1)||
                // (k==73&&i==term_txt.length()-2&&last_s==73)){
                if (sbf == null) {         //the first time meet a digit   hubin
                    sbf = new StringBuffer();
                    sbf.append(s);
                    dig_status = true;
                } else {                  //not the first time meet a digit    hubin
                    if (dig_status) {
                        sbf.append(s);
                    } else {                                 				
                        llist.add(new subterm(sbf.toString(), 0, q));
                        sbf = new StringBuffer();
                        sbf.append(s);
                        dig_status = true;
                    }
                }
            } else {
                if (sbf == null) {
                    sbf = new StringBuffer();
                    sbf.append(s);
                    dig_status = false;
                } else {
                    if (dig_status) {
                        llist.add(new subterm(sbf.toString(), 2, q));
                        if (k != 35) {                              //char 35 is #
                            sbf = new StringBuffer();
                            sbf.append(s);
                            dig_status = false;
                        }
                    } else {
                        if (k != 35) {	
                            sbf.append(s);
                        } else {
                            llist.add(new subterm(sbf.toString(), 0, q));
                        }
                    }
                }
            }
        }
        return llist;

    }

  /* called by breaking:  case_breaking(term            term_after_digit_break, 
   *                                    m               ith term after latin_breaking, 
   *                                    n               jth term after digit_breaking, 
   *                                    latin_size      the nuber of terms after latin_breaking, 
   *                                    digit_size      the nuber of terms after digital_breaking, 
   *                                    term_t          term before breaking, 
   *                                    q               the number of the position of the term before breaking);    ---hubin 
   *                                    
   *  If the term is a digit string or a latin or I+, add into the return linkedlist directly, skip case_breaking
   * (0)  if the word starts with and end with a uppercase, change the beginning char to lowercase                                 
   * (1) seperate by lowercase part and uppercase part
   * (2) in lowercase part:
   *       (2-1) if (reach_last && islast) and ( the length of this part before this char is >= 3)
   *              (2-1-1) if (cause different stem),  the part before this char is a new substem, type 3;
   *                                                  the current lowercase char is a new substem. type 9.
   *              (2-1-2) if ( not cause different stem), the current char is appended to the before part.
   *       (2-2) if (not reach_last && islast) or (the length of this part before this char is < 3), 
   *                                                   the current char is appended to the before part.
   * (3) in uppercase part:
   *       (3-1) if (reach_last && islast) and (current char is the first uppercase)
   *                     the current upperercase char is a new substem. type 9.
   *       (3-2) if (!reach_last && islast) or ( the length of the part before it < 3)
   *                     append current char to the part before it if one or more uppercase chars just before it; else build a new buffer for it.
   *       (3-3) if (reach_last && islast) and (length > 3) and (cause different stem)
   *                      the part before this char is a new substem, type 4;
   *                     the current upperercase char is a new substem. type 9.
   *       (3-4) otherwise, append
   *       
   * Summary:
   * type 3: the string contain only lowercase chars 
   * type 4: the string contain only uppercase chars 
   * type 9: the char satisfies: (1) (reach_last && islast) 
   *                             (2) (cause different stem)
   *                             (3) (the length of the part of before this char > 3)
   * Note: the stem is checked by the super-parse method of okapi
   *                             
   * Return: a linkedlist of subterm                            
   *                                                         ---hubin 
   */
    public LinkedList case_breaking(subterm term, int m, int n, int latin_size, int digit_size, String term_t, int q) {
   //     System.out.println("Case Break: " + term.text());
        String termtext = term_t.toLowerCase();
        float weight_c = mybss.weight(term.text());
        boolean islast = term.text().length() >= 4 && (weight_c > 4.0f)             
                && (!termtext.equals("type")) && (!termtext.equals("gene")) && (!termtext.equals("receptor")) && (!termtext.equals("protein")) 
                && term_t.length() >= 4     //it is not used in fact, because if the condition "term.text().length() >= 4" is true, it always true
                && ((m == latin_size - 1 && (n == digit_size - 2) && term_t.matches(".*[a-zA-Z]+[0-9]+"))
                || (m == latin_size - 1 && (n == digit_size - 1)  && term_t.matches(".*[a-zA-Z]+")));
                
        System.out.println("PHRASEPHRASE case_breaking    test islast.  \"islast = \"" + islast 
        		           +  "   \"term.text() = \"" + term.text()
        		           +  "   \"term_t = \"" + term_t
        		           +  "   \"latin_size = \"" + latin_size
        		           +  "   \"digit_size = \"" + digit_size
        		           +  "   \"m = \"" + m
        		           +  "   \"n = \"" + n);   
             
        LinkedList llist = new LinkedList();
        subterm curr_term = term;
        String term_txt = curr_term.text();

        //if the word is a digit string or a latin or I+ which is found in  BigDigitbreak(); 
        if (curr_term.type > 0 || term_txt.matches("I+")) {
            llist.add(curr_term);
            return llist;
        }
       // if the word starts with and end with a uppercase, change the beginning char to lowercase  !!!!!!!!!
        if (term_txt.matches("[A-Z][a-z]+[a-zA-Z]*")) {       
            term_txt = term_txt.substring(0, 1).toLowerCase()
                    + term_txt.substring(1);
        }
	 
        StringBuffer sbf = null;   //is used to record current part
        boolean upper_status = false;

        term_txt = term_txt + "#";
        for (int i = 0; i < term_txt.length(); i++) {
            boolean reach_last = i == term_txt.length() - 2; //whether the current char is the last char or not.
            char s = term_txt.charAt(i);
            int k = (int) s;
            
            if (k >= 65 && k <= 90) {                //uppercase char
                if (sbf == null) {
                    sbf = new StringBuffer();
                    sbf.append(s);
                    upper_status = true;
                } else {
                    if (upper_status) {  //before this char, there are already some uppercase char in current part
                        if (!(reach_last && islast)) {  // this char is neither at end of the current subterm nor "islast"
                            sbf.append(s);
                        } else {                     
                        	 System.out.println("PHRASEPHRASE  case_breaking, test sbf, can accessable here? " 
                 	              	+  "   \"sbf = \"" + sbf.toString()
                 	            	+  "   \"current char s = \"" + s);    //tested, yes!!!
                            if ((sbf.length() == 3) || sbf.length() > 3) {
                                String y1 = sbf.toString();
                                String y2 = new StringBuffer(sbf.toString()).append(s).toString();
                                if (mybss.isStemSame(y1, y2)) {           //if the stem of this part is not changed, just append it
                                    sbf.append(s);	               
                                    System.out.println("PHRASEPHRASE  case_breaking, test y1, y2 , can accessable here? " 
                        	              	+  "   \"y1 = \"" + y1
                        	            	+  "   \"y2 = \"" + y2
                        	            	+  "   \"s = \"" + s);        //tested, NO!!!
                                } else {                                   //if the stem of this part is changed:  (condition: (1)length > 3 (2) stem changed)
                                	                                      //since islast, the current only can be the last char of current term.
                                    llist.add(new subterm(sbf.toString(), 4, q));   //the before part is saved as a new sub term, type is 4  only upprecase chars inside!!!!!!!!
                                    StringBuffer lastchar = new StringBuffer().append(s);
                                    llist.add(new subterm(lastchar.toString(), 9, q)); //the current uppercase char is used to build a new subterm, which will cause a different stem, type is 9 !!!!!!
                                    sbf = null;                              //start a new emprty string buffer
                                    continue;
                                }
                            } else {               //if current sbf length < 3, just append
                                sbf.append(s);				
                            }
                        }
                    } else {      //there are only some non-uppercase char in sbf, and current char is the first uppercase char in this part.
                        llist.add(new subterm(sbf.toString(), 3, q)); //the before part is saved as a new sub term, type is 3, all chars in this part should be lowercase!!!!!!!!
                        sbf = new StringBuffer();
                        sbf.append(s);             //strat a new buffer to store a new part
                        upper_status = true;
                        if (reach_last && islast) {                     
                            StringBuffer lastchar = new StringBuffer().append(s);
                            llist.add(new subterm(lastchar.toString(), 9, q)); //if reach_last && islast, the current char is used to build a new part, type is 9
                            sbf = null;
                            upper_status = false;
                            continue;						
                        }
                    }
                }
            } else {                //lowcase char
                if (sbf == null) {
                    if (k != 35) {
                        sbf = new StringBuffer();
                        sbf.append(s);
                        upper_status = false;
                    }
                } else {
                    if (upper_status) {  //already has uppercase in this part
                        llist.add(new subterm(sbf.toString(), 4, q)); //The before part which only contains uppercase chars, build a new subterm, type 4
                        if (k != 35) {
                            sbf = new StringBuffer();             //new buffer for storing current lowcase char
                            sbf.append(s);
                            upper_status = false;
                        }
                    } else {         //if no uppercase in this part
                        if (k != 35) {
                            if (!(reach_last && islast)) {	
                                sbf.append(s);
                            } else {         //if reach_last && islast
                                if ((sbf.length() == 3) || sbf.length() > 3) {
                                    String y1 = sbf.toString();
                                    String y2 = new StringBuffer(sbf.toString()).append(s).toString();

                                    if (mybss.isStemSame(y1, y2)) {
                                        sbf.append(s);										
                                    } else {                              // the current char is a lowercase char and will cause a different stem
                                        llist.add( new subterm(sbf.toString(), 3, q));   //the before part is stored as a new part, only lowercase char inside, type 3
                                        StringBuffer lastchar = new StringBuffer().append(s);

                                        llist.add( new subterm(lastchar.toString(), 9, q)); //the current lowercase char is used to build a new subterm, which will cause a different stem, type is 9 !!!!!!
                                        sbf = null;
                                        continue;
                                    }
                                } else {
                                    sbf.append(s);
								
                                }
                            }
                        } else {
                            llist.add(new subterm(sbf.toString(), 3, q));
                        }
                    }
                }
            }
        }
        return llist;	
    }

    //do latin_breaking, digit_breaking and case_breaking to each word
    public LinkedList breaking(String term, int q) {
        LinkedList break_seq = new LinkedList();
        String curr_term = term;

        if (term.matches(".*[A-Z0-9]s")) {
            curr_term = term.substring(0, term.length() - 1);
        }
        LinkedList latin_br = latin_breaking(curr_term, q);

        for (int i = 0; i < latin_br.size(); i++) {
            subterm b_term = (subterm) (latin_br.get(i));
            LinkedList digit_br = digit_breaking(b_term, q);

            for (int j = 0; j < digit_br.size(); j++) {
                subterm d_term = (subterm) (digit_br.get(j));

       //         System.out.println(new Integer(j) + " DO: " + d_term.text());
                LinkedList case_br = case_breaking(d_term, i, j, latin_br.size(), digit_br.size(), term, q);

                for (int m = 0; m < case_br.size(); m++) {
                    subterm c_term = (subterm) (case_br.get(m));

     //               System.out.println(new Integer(m) + " NO: " + c_term.text());
                    break_seq.add(c_term);
                }

            }
        }
        return break_seq;
    }

/*
    public LinkedList joiner(subterm term, int k, int w) {
        int upper = k;

        subterm s_term = term;
        int shift_distance = 32 - subterm_seq.size() + 1;
        LinkedList ll = new LinkedList();

        for (int i = 0; i < s_term.size(); i++) {
            upper = k;
            while (upper >= 0) {
                int shift_upper = upper;

                shift_upper = shift_upper << shift_distance;
                String link = null;
                String previous_tok = null;

                for (int j = 0; j < subterm_seq.size(); j++) {
                    subterm curr_term = (subterm) (subterm_seq.get(j));
                    String tok;

                    if (curr_term == s_term) {
                        tok = s_term.get(i);
                    } else {
                        tok = curr_term.get(0);
                    }
                    if (previous_tok == null) {
                        previous_tok = tok;
                        link = tok;
                        continue;
                    } else {
                        int test_bit = shift_upper & w;

                        if (test_bit == w) {
                            // if(test_bit==w&&(!ifstart)){
                            link = link.trim() + tok.trim();
                            previous_tok = tok;
                        } else {
                            link = link.trim() + " " + tok.trim();
						
                            // System.out.println("LINKLINK:"+link);

                            previous_tok = tok;
							
                        }
                        shift_upper = shift_upper << 1;

                    }
                }
                if (mybss.validate(link) && !ll.contains(link)) {
                    ll.add(link);
                }			
                // if(!ll.contains(link))	ll.add(link);			
                upper--;
            }

        }
        return ll;
    }
*/
  
    /* deleted by hubin, not used in fact
    public LinkedList creat_searchset(LinkedList combination, float discount) {
        LinkedList sets = new LinkedList();
  //      String comb1 = print_combination(combination);            //deleted by hubin

   //     System.out.println("Creating search set for " + comb1);
        // if(comb1.matches(".*[a-zA-Z0-9]+ [a-zA-Z0-9]+.*")){
        adj_set = mybss.adjsearch(combination, discount);
        sets.add(adj_set);
   //     System.out.println( "adj set:" + new Integer(adj_set.setno) + " " + new Float(adj_set.weight));
        return sets;
    }
*/
    
    /*   deleted by hubin, never be used
    phrase(String text_t, relex rx, LinkedList ll) {

   //     System.out.println("Construting Phrase: " + text_t);
        x = rx;
        mybss = new bss(x);

        search_list = new LinkedList();
        term_seq = new LinkedList();
        StringTokenizer sto = new StringTokenizer(text_t);

        while (sto.hasMoreTokens()) {
            term_seq.add(sto.nextToken());
        }

        subterm_combination1 = ll;
        search_list = new LinkedList();
        if (subterm_combination1 != null && subterm_combination1.size() > 0) {

            LinkedList sets1 = creat_searchset(subterm_combination1);

            search_list.addAll(sets1);
        }
		
    }
*/
 /*    deleted by hubin, never be used
    public void merge(phrase phr) {
  //      System.out.println("Merging.....");

        search_list = new LinkedList();
        int term_seq_base = term_seq.size(); 

        term_seq.addAll(phr.term_seq);
        for (int i = 0; i < phr.subterm_seq.size(); i++) {
            subterm sbt = (subterm) (phr.subterm_seq.get(i));

            sbt.term_num = sbt.term_num + term_seq_base;
            subterm_seq.add(sbt);
        }
        subterm_combination1.addAll(phr.subterm_combination1);

        if (subterm_combination1 != null && subterm_combination1.size() > 0) {

            LinkedList sets1 = creat_searchset(subterm_combination1);

            search_list.addAll(sets1);

        }
	
    }
*/   
  
    /*
    public void printtermlink(LinkedList aa, String fr) {
        for (int i = 0; i < aa.size(); i++) {
            String thisterm = ((String) (aa.get(i))).toLowerCase().trim();

            if (fr.matches("^" + aa + "\\+.*")
                    || fr.matches(".*\\+" + aa + "\\+.*")) {
           //     System.out.println("XXX:" + fr);
            } else {
          //      System.out.println("XXX:" + thisterm + "+" + fr);
            }
        }
    }
*/
    /*
    phrase(String text_t, relex rx, float discount) {
      //  System.out.println("Construting Phrase: " + text_t);
        x = rx;
        mybss = new bss(x);
        term_seq = new LinkedList();

        search_list = new LinkedList();
        StringTokenizer sto = new StringTokenizer(text_t);

        while (sto.hasMoreTokens()) {
            term_seq.add(sto.nextToken());
        }

        BigDigitbreak();
        subtermize();
        

   //     subterm_seq = new Infokit().variant_creator(subterm_seq);           
 //       print_subterm_seq();            // deleted by hubin

        subterm_combination1 = combine(true, false);
        System.out.println("DDDDDDDDDDDDDDDDDDD phase class: check method \"combine\"");                                      //add by hubin
        for(int i = 0; i < subterm_combination1.size(); i++)                                                                  //add by hubin
        	System.out.println("DDDDDDDDDDDDDDDDDDD subterm_combination1 [" + i + "] is:  "  + subterm_combination1.get(i));  //add by hubin
  //      System.out.println("Combine1:" + print_combination(subterm_combination1));
        if (subterm_combination1 != null && subterm_combination1.size() > 0) {
	
            LinkedList sets1 = creat_searchset(subterm_combination1, discount);

            search_list.addAll(sets1);
        }
		
    }
*/
    
    /*
    public void append(String text) {
        if (term_seq == null) {
            term_seq = new LinkedList();
        }
        term_seq.add(text);
        return;
    }
*/
   /* 
    public String toString() {
        String result = "";

        for (int i = 0; i < term_seq.size(); i++) {
            result = result + (String) (term_seq.get(i)) + " ";
        }

        return result.trim();
    }*/
}

