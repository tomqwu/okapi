import java.util.*;

public class bss {
    public relex x;
    
    bss(relex rx) {
        x = rx;
    }

    public boolean isStemSame(String x1, String x2) {
        String y1 = superparse(x1).trim();
        String y2 = superparse(x2).trim();
        return y1.equals(y2);
    }

    public String superparse(String str) {
    	 	      System.out.println("BSSBSSBSS superparse():  str =  " + str);
        String result = x.comm("sp t=" + str.trim().toLowerCase());
                  System.out.println("BSSBSSBSS superparse(): (1) result = " + result);          
        result = result.replaceAll("^[0-9]*", "").replaceAll("t=[\\w]* c=H s=[\\w]*", "");
                  System.out.println("BSSBSSBSS superparse(): (2) result = " + result); 
        String finalr = result.replaceAll("c=[NS] s=[\\w]*", "").replaceAll("t=", " ");  
                  System.out.println("BSSBSSBSS superparse(): finalr  = " + finalr ); 
        if (finalr.trim().equals("")) {
            return "";
        }
        return finalr;
    }
//?????????? check??
    public float weight(String str) {
    	      //   System.out.println("**************** str in weight of bss: sp t=\n " + str);
        String result = x.comm("sp t=" + str.trim().toLowerCase());           //call super parese, get the return string. hubin
               //   System.out.println("**************** 1 result in superparse of bss: sp t=\n " + result);
        result = result.replaceAll("^[0-9]*", "").replaceAll("t=[\\w]* c=H s=[\\w]*", "");
        String finalr = result.replaceAll("c=[NS] s=[\\w]*", "").replaceAll("t=", " ");        

        if (finalr.trim().equals("")) {
            return 0.0f;
        }
               //     System.out.println("**************** finalr in weight of bss: f t=\n " + finalr);
        result = x.comm("f t=" + finalr.trim().toLowerCase());
              //     System.out.println("**************** 2 result in weight of bss: f t=\n " + result);
        String[] parsed = result.split(" np=");
        String[] parsed2 = parsed[0].split("S");
        String setnum = parsed2[1];
        String totalnp = parsed[1];
        String[] parsed3 = totalnp.split("t=");
        		
               //   System.out.println("**************** parsed3[0] in weight of bss: w n=\n " + parsed3[0]);
        result = x.comm("w n=" + parsed3[0] + " r=0 bigr=0");
               //    System.out.println("**************** 3 result in weight of bss: w n=\n " + result);
        float adj_weight = new Float(result).floatValue();

        new bssset(new Integer(setnum).intValue()).destroy(x);
        return adj_weight;

    }

    /*
     * called two times:
     * (1) in the combine() method to check the each option words
     */
    public boolean validate(String vali) {
    	String result = x.comm("sp a=tx t=" + vali);
	System.out.println("DAMON>>>validate" + "sp a=tx t=" + vali);
    	      System.out.println("BSSBSSBSS validate(): (1) result = " + result  );
        result = result.replaceAll("^[0-9]*", "").replaceAll("t=[\\w]* c=H s=[\\w]*", "");  //remove all "c=H" word; remove the beginning digit
              System.out.println("BSSBSSBSS validate(): (2) result = " + result  );
        String finalr = result.replaceAll("c=[NS] s=[\\w]*", "").replaceAll("t=", " ");
              System.out.println("BSSBSSBSS validate(): (3) finalr = " + finalr  );
     
        StringTokenizer ph = new StringTokenizer(finalr);
        StringBuffer mydellist = new StringBuffer();
        StringBuffer termlist = new StringBuffer();

        while (ph.hasMoreTokens()) {
            String command = "f t=" + ph.nextToken();
            result = x.comm(command);
                      System.out.println("BSSBSSBSS validate(): (4) command = " + command  );
                      System.out.println("BSSBSSBSS validate(): (5) result  = " + result  );  
            String[] parsed = result.split("np=");
            String[] parsed2 = parsed[0].split("S");
            String setnum = parsed2[1];

            parsed2 = parsed[1].split(" t=");
            mydellist.append(" s=" + setnum);
            termlist.append(" s=" + setnum);
        }
                      System.out.println("BSSBSSBSS validate(): (6) mydellist = " + mydellist  );
                      System.out.println("BSSBSSBSS validate(): (7) termlist =  " + termlist  );
        if (termlist.toString().trim().equals("")) {
            return false;
        }
        result = x.comm("f " + termlist.toString() + " op=adj");
                      System.out.println("BSSBSSBSS validate(): (8) result = " + result  );
        String[] parsed = result.split(" np=");
        String[] parsed2 = parsed[0].split("S");
        String setnum = parsed2[1];
        int num = new Integer(parsed[1].trim()).intValue();

        mydellist.append(" s=" + setnum);
                      System.out.println("BSSBSSBSS validate(): (9) mydellist = " + mydellist  );
        x.comm("delete " + mydellist);
            
        if (num > 0) {
            return true;
        } else {
            return false;
        }
    }
    
//passed the parameter: the linkedlist "subterm_combination1" which is built after combine() in pharse class
    public bssset adjsearch(LinkedList subterm_combi) {
        bssset b1 = condition_search(subterm_combi, "adj");
        b1.putreal_comb(subterm_combi);
        return b1;		
    }
    
/* not used in fact, hubin
    public bssset adjsearch(LinkedList subterm_combi, float discount) {
        bssset b1 = condition_search(subterm_combi, "adj", discount);

        b1.putreal_comb(subterm_combi);

        return b1;		
    }
*/
    
/*
 * passed the parameter: the linkedlist contains breaked word and combined option; op is adj which is defned in okapi
 * 
 * called for building bssset basing on "subterm_combination1" 
 * -->bss.adjsearch()-->phrase.creat_searchset()-->phrase.phrase()-->snippet.snippet()-->snippet_maker-->squery.squery()
 */  
    
    public bssset condition_search(LinkedList subterm_seq, String op) {
    	                  System.out.println("BSSBSSBSS condition_search()(1) passed parameter:  subterm_seq= " + subterm_seq.toString() + ";;;; op =" + op);
        StringBuffer phraselist = new StringBuffer();
        StringBuffer dellist = new StringBuffer();

        for (int i = 0; i < subterm_seq.size(); i++) {
            String phrase1 = (String) (subterm_seq.get(i));
            StringTokenizer ph = new StringTokenizer(phrase1);
            StringBuffer termlist = new StringBuffer();

            while (ph.hasMoreTokens()) {
                String preterm = ph.nextToken();
                String result = x.comm("sp t=" + preterm);
                         System.out.println("BSSBSSBSS condition_search()(1)  command 1-0:    sp t= " + preterm);
                         System.out.println("BSSBSSBSS condition_search()(1)  result 1-0:           " + result);
                String[] parsed = result.split("t=");	
                String[] parsed2 = parsed[1].split(" c=");
                String term = parsed2[0];

                if (preterm.startsWith("&")) { 
                    term = preterm.replaceAll("&", "");
                }
                String findcommand = "f t=" + term;
                String findresult = x.comm(findcommand);
                       System.out.println("BSSBSSBSS condition_search()(1)  command 1-1:    " + findcommand);
                       System.out.println("BSSBSSBSS condition_search()(1)  result 1-1:     " + findresult);
                parsed = findresult.split(" np=");
                parsed2 = parsed[0].split("S");
                String setnum = parsed2[1];

                parsed2 = parsed[1].split(" t=");
     

                dellist.append(" s=" + setnum);
                termlist.append(" s=" + setnum);
				
            }
            String command = "f" + termlist.toString() + (op == null ? "" : (" op=" + op));  //Because op always is "adj", it is no meaningful                               
            String result = x.comm(command);
                   System.out.println("BSSBSSBSS condition_search()(1)   command 2 :    " + command);
                   System.out.println("BSSBSSBSS condition_search()(1)   result 2:      " + result);
            String[] parsed = result.split(" np=");
            String[] parsed2 = parsed[0].split("S");
            String setnum = parsed2[1];

            dellist.append(" s=" + setnum);
            phraselist.append(" s=" + setnum);
			
        }
		
        String command3 = "f" + phraselist.toString() + " op=or";
        String result = x.comm(command3);
                  System.out.println("BSSBSSBSS condition_search()(1)    command 3:    " + command3);
                  System.out.println("BSSBSSBSS condition_search()(1)    result  3:    " + result);
        String[] parsed = result.split(" np=");
        String[] parsed2 = parsed[0].split("S");
        String setnum = parsed2[1];
        String totalnp = parsed[1].trim();

        String command4 = "w n=" + totalnp + " r=0 bigr=0";
        result = x.comm(command4);
             System.out.println("BSSBSSBSS condition_search()(1)    command 4:    " + command4);
             System.out.println("BSSBSSBSS condition_search()(1)    result  4:    " + result);
        float weight = new Float(result).floatValue();

        x.comm("delete " + dellist);             
        bssset bst = new bssset(new Integer(setnum).intValue(), new 
                Integer(totalnp).intValue(), weight);

        bst.putreal_num(new Integer(totalnp).intValue());  //it should be no uesful.the real_num already be assigned in the above constructor call
                System.out.println("BSSBSSBSS condition_search()(1) return:   bst= " +  bst.toString());
        return bst; 

    }

    /*
     * called by fb_qoutput.  "bssset bst = new bss(x).condition_search(ll3, null, wei.floatValue());"
     * 
     */
    public bssset condition_search(LinkedList subterm_seq, String op, float discount) {
    	       System.out.println("BSSBSSBSS condition_search()(2) begining passed parameter:  subterm_seq= " + subterm_seq.toString() + ";;;;   op =" + op + ";;;;   discount =" + discount);
        StringBuffer phraselist = new StringBuffer();
        StringBuffer dellist = new StringBuffer();

        for (int i = 0; i < subterm_seq.size(); i++) {
            String phrase1 = (String) (subterm_seq.get(i));
            StringTokenizer ph = new StringTokenizer(phrase1);
            StringBuffer termlist = new StringBuffer();

            while (ph.hasMoreTokens()) {
                String preterm = ph.nextToken();
                String result = x.comm("sp t=" + preterm);
                  //    System.out.println("****************  command in condition_search of bss:    sp t= + preterm  \n" + preterm);
                String[] parsed = result.split("t=");	
                String[] parsed2 = parsed[1].split(" c=");
                String term = parsed2[0];

                if (preterm.startsWith("&")) { 
                    term = preterm.replaceAll("&", "");
                }
                parsed = x.comm("f t=" + term).split(" np=");
                  //     System.out.println("****************  command in condition_search of bss:    f t= + term  \n" + term);
                parsed2 = parsed[0].split("S");
                String setnum = parsed2[1];

                parsed2 = parsed[1].split(" t=");


                dellist.append(" s=" + setnum);
                termlist.append(" s=" + setnum);
				
            }
            String command = "f" + termlist.toString() + (op == null ? "" : (" op=" + op));
            String result = x.comm(command);
                  //       System.out.println("****************  command in condition_search of bss:    f + termlist.toString() + (op == null ? \"\" : ( op= + op))  \n" + command);
            String[] parsed = result.split(" np=");
            String[] parsed2 = parsed[0].split("S");
            String setnum = parsed2[1];

            dellist.append(" s=" + setnum);
            phraselist.append(" s=" + setnum);
			
        }
		
        String result = x.comm("f" + phraselist.toString() + " op=or");
         //  System.out.println("****************  command in condition_search of bss:   \"f\" + phraselist.toString() + \" op=or\"  \n" + phraselist.toString());
        String[] parsed = result.split(" np=");
        String[] parsed2 = parsed[0].split("S");
        String setnum = parsed2[1];
        String totalnp = parsed[1].trim();

        result = x.comm("w n=" + totalnp + " r=0 bigr=0");
         //  System.out.println("****************  command in condition_search of bss:   \"w n=\" + totalnp + \" r=0 bigr=0\"  \n" + totalnp);
        float weight = new Float(result).floatValue() * discount;   // the passed "adjusted" is used here

        x.comm("delete " + dellist);
         //  System.out.println("****************  command in condition_search of bss:   \"delete \" + dellist  \n" + dellist);
        bssset bst = new bssset(new Integer(setnum).intValue(), new 
                Integer(totalnp).intValue(), weight);

        bst.putreal_num(new Integer(totalnp).intValue()); //no useful, already defined in above bssset constructor
        bst.put_discount(discount);                         //no useful, already defined in above bssset constructor
        LinkedList lst = new LinkedList();

        lst.add(subterm_seq.get(0));
        bst.putreal_comb(lst);
        
        System.out.println("BSSBSSBSS condition_search()(2) return:   bst= " +  bst.toString());
        return bst; 

    }

}

