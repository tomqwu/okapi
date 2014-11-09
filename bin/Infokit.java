import java.io.*;
import java.util.*;

public class Infokit {
    public Hashtable concept_word_hashtable;
    public Hashtable stopword;
    public Hashtable half_stop;
  public Hashtable ab_pair;
    
    public String[] latin_group = {
        "alpha", "Alpha", "ALPHA", "beta", "Beta",
        "BETA", "gamma", "Gamma", "GAMMA", "epsilon", "Epsilon", "EPSILON",
        "kappa", "Kappa", "KAPPA"};

    Infokit() {
        try {
            construct_option();
            construct_stopword();           //hubin
            construct_halfstop();
            ab_pair = new Hashtable();     
        } catch (Exception ee) {}
    }

   public void construct_option() {
	   // build a hashtable, the front one is the key; the second element is a linkedlink to hold the concept
        concept_word_hashtable = new Hashtable();
        concept_word_hashtable.put("alpha", new Concept_words_LinkedList().add("a"));
        concept_word_hashtable.put("beta", new Concept_words_LinkedList().add("b"));
        concept_word_hashtable.put("gamma", new Concept_words_LinkedList().add("g"));
        concept_word_hashtable.put("epsilon", new Concept_words_LinkedList().add("e"));
        concept_word_hashtable.put("kappa", new Concept_words_LinkedList().add("k"));
        concept_word_hashtable.put("Alpha", new Concept_words_LinkedList().add("a"));
        concept_word_hashtable.put("Beta", new Concept_words_LinkedList().add("b"));
        concept_word_hashtable.put("Gamma", new Concept_words_LinkedList().add("g"));
        concept_word_hashtable.put("Epsilon", new Concept_words_LinkedList().add("e"));
        concept_word_hashtable.put("Kappa", new Concept_words_LinkedList().add("k"));
        concept_word_hashtable.put("ALPHA", new Concept_words_LinkedList().add("a"));
        concept_word_hashtable.put("BETA", new Concept_words_LinkedList().add("b"));
        concept_word_hashtable.put("GAMMA", new Concept_words_LinkedList().add("g"));
        concept_word_hashtable.put("EPSILON", new Concept_words_LinkedList().add("e"));
        concept_word_hashtable.put("KAPPA", new Concept_words_LinkedList().add("k"));
        concept_word_hashtable.put("a", new Concept_words_LinkedList().add("alpha"));
        concept_word_hashtable.put("b", new Concept_words_LinkedList().add("beta"));
        concept_word_hashtable.put("g", new Concept_words_LinkedList().add("gamma"));
        concept_word_hashtable.put("e", new Concept_words_LinkedList().add("epsilon"));
        concept_word_hashtable.put("k", new Concept_words_LinkedList().add("kappa"));
        concept_word_hashtable.put("A", new Concept_words_LinkedList().add("alpha"));
        concept_word_hashtable.put("B", new Concept_words_LinkedList().add("beta"));
        concept_word_hashtable.put("G", new Concept_words_LinkedList().add("gamma"));
        concept_word_hashtable.put("E", new Concept_words_LinkedList().add("epsilon"));
        concept_word_hashtable.put("K", new Concept_words_LinkedList().add("kappa"));
        // htb.put("I",new opset().add("1"));
        // htb.put("1",new opset().add("I"));
         //htb.put("I",new opset().add("1"));
         //htb.put("1",new opset().add("I"));
        concept_word_hashtable.put("II", new Concept_words_LinkedList().add("2"));
        concept_word_hashtable.put("2", new Concept_words_LinkedList().add("II"));
        concept_word_hashtable.put("III", new Concept_words_LinkedList().add("3"));
        concept_word_hashtable.put("3", new Concept_words_LinkedList().add("III"));
        concept_word_hashtable.put("p", new Concept_words_LinkedList().add("protein"));
        concept_word_hashtable.put("r", new Concept_words_LinkedList().add("receptor"));
        concept_word_hashtable.put("P", new Concept_words_LinkedList().add("protein"));
        concept_word_hashtable.put("R", new Concept_words_LinkedList().add("receptor"));

        // htb.put("protein",new opset().add("P"));
        // htb.put("receptor",new opset().add("R"));
        // htb.put("Protein",new opset().add("P"));
        // htb.put("Receptor",new opset().add("R"));
        concept_word_hashtable.put("mutation", new Concept_words_LinkedList().add("mutant"));
        concept_word_hashtable.put("Mutation", new Concept_words_LinkedList().add("mutant"));
        concept_word_hashtable.put("mutations", new Concept_words_LinkedList().add("mutant"));
        concept_word_hashtable.put("type", new Concept_words_LinkedList().add(""));
        concept_word_hashtable.put("Type", new Concept_words_LinkedList().add(""));
        concept_word_hashtable.put("TYPE", new Concept_words_LinkedList().add(""));
	
        concept_word_hashtable.put("gene", new Concept_words_LinkedList().add("genetic").add(""));
        concept_word_hashtable.put("Gene", new Concept_words_LinkedList().add("genetic").add(""));
        concept_word_hashtable.put("GENE", new Concept_words_LinkedList().add("genetic").add(""));
        concept_word_hashtable.put("genes", new Concept_words_LinkedList().add("genetic").add(""));
        concept_word_hashtable.put("Genes", new Concept_words_LinkedList().add("genetic").add(""));
        concept_word_hashtable.put("GENES", new Concept_words_LinkedList().add("genetic").add(""));
	
        // htb.put("subunit",new opset().add(""));
        // htb.put("Subunit",new opset().add(""));
        // htb.put("SUBUNIT",new opset().add(""));

    }
// delete by hubin, the file "stopword2" is not exist, the stopword always is empty
    public void construct_stopword() throws Exception {
        stopword = new Hashtable();
        File stopFile = new File("stopword2"); 
        BufferedReader input = null;

        try {
            input = new BufferedReader(new FileReader(stopFile));
            String line = null; 

            while ((line = input.readLine()) != null) {
                if (line.trim().equals("")) {
                    continue;
                }
                stopword.put(line.trim(), "");
            }
        } catch (Exception e) {}
    }

    public void construct_halfstop() {
        half_stop = new Hashtable();
        half_stop.put("gene", "");
        half_stop.put("genes", "");
        half_stop.put("disease", "");
        half_stop.put("diseases", "");
        half_stop.put("mutation", "");
        half_stop.put("mutations", "");
    }
    
    public LinkedList variant_creator(LinkedList ll) {
        LinkedList terms = new LinkedList();
        LinkedList term_seq = ll;

        for (int i = 0; i < term_seq.size(); i++) {
            subterm sbt = (subterm) (term_seq.get(i));
            String txt = (String) (sbt.get(0));
            Object opt = concept_word_hashtable.get(txt);

            if (opt != null) {
                LinkedList llt = (LinkedList) (((Concept_words_LinkedList) opt).concept_words_linkedlist);

                for (int j = 0; j < llt.size(); j++) {
                    sbt.add_option((String) (llt.get(j)));
                }
            }
            terms.add(sbt);		
        }
        return terms;
    }
   
 //it do not change anything in fact
    public void add_ab(Hashtable htb1) {
        ab_pair.putAll(htb1);
        return;
    }
  
}

