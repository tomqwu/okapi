
import java.util.*;

public class snippet {
    public relex x;
    public phrase[] spr;
    public String[] tokens;
    
    snippet(String text_snip, relex rx) {
        x = rx;
        tokens = tokenizer(text_snip);
        spr = phrasize(tokens);   //important, call phase
    }

    public String toString() {
    	String snippetobject= "";
    	for(int i = 0; i < tokens.length; i++){
        	snippetobject = snippetobject + "SNIPPETSNIPPET object  tokens[" + i + "]" + tokens[i].toString() + "\n";
        }
        for(int i = 0; i < spr.length; i++){
        	snippetobject = snippetobject + "SNIPPETSNIPPET object  phrase[" + i + "]" + spr[i].toString() + "\n";
        }
        return snippetobject;
    }

    /* deleted by hubin, never used
    snippet(String text_snip1, String text_snip2, relex rx) {
        x = rx;
        tokens = tokenizer(text_snip1);

        phrase[] spr1 = new phrase[1];
        String fullname = text_snip1.replaceAll("-", " ");

        spr1[0] = new phrase(fullname, x);

        phrase[] spr2 = new phrase[1];

        spr2[0] = new phrase(text_snip2.replaceAll("-", " "), x);

        spr1[0].merge(spr2[0]);
        LinkedList ll2 = new LinkedList();
        LinkedList ll1 = fullname_terms(fullname);

        ll2.add(spr1[0]);
        ll2.addAll(ll1);
        spr = new phrase[ll2.size()];
        for (int j = 0; j < ll2.size(); j++) {
            spr[j] = (phrase) (ll2.get(j));
        }
       
    }
*/
    /* deleted by hubin, never used
    public LinkedList fullname_terms(String qry) {
        LinkedList ll1 = new LinkedList();
        StringTokenizer stokens = new StringTokenizer(qry);

        while (stokens.hasMoreTokens()) {
            String kk = stokens.nextToken();      
             phrase pp = new phrase(kk,x,0.41f);          
            ll1.add(pp);
        }
        return ll1;
    }
*/
    
    /*
     * call getsearchsets methods in phrase class whcih  return a linkedlist of bssset objects
     * A snippet hold an array of pharse, get their bsssets one by one. 
     */
    public LinkedList getsearchsets() {
        LinkedList ll1 = new LinkedList();

        for (int i = 0; i < spr.length; i++) {
            phrase phr = spr[i];

            ll1.addAll(phr.getsearchsets());
        }
        return ll1;
    }
/*                       //deleted by hubin
    public String toString() {
        StringBuffer sbt = new StringBuffer();

        for (int i = 0; i < spr.length; i++) {
            sbt.append(spr[i].toString().trim() + " ");
        }
        return sbt.toString().trim();
    }
*/
    
    
    public phrase[] phrasize(String[] toks) {
        int i = 0;
        LinkedList ll = new LinkedList();

        while (i < toks.length) {
            phrase phr = new phrase(toks[i++].replaceAll("-", " "), x);
            ll.add(phr);
        }	
        phrase[] phra_seq = new phrase[ll.size()];
        int size = ll.size();

        for (int j = 0; j < size; j++) {
            phra_seq[j] = (phrase) (ll.removeFirst());
     //       System.out.println("Phrase: " + phra_seq[j]);
        }
        return phra_seq;
    }

    public String[] tokenizer(String snp) {
        StringTokenizer sto = new StringTokenizer(snp.replaceAll("'s", ""), " -'/");
        String[] toks = new String[sto.countTokens()];
        int n = 0;

        while (sto.hasMoreTokens()) {
            toks[n] = sto.nextToken();
            n++;
        }
        return toks;	
    }

}

