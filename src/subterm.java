
import java.util.*;

public class subterm {
    public int type;
    public String text;
    public LinkedList option;
    public String option_rep;
    public LinkedList superstring;
    public int term_num;
    public String original;
    subterm(String txt, int typ, int q) {
        text = txt;
        term_num = q;
        type = typ;
        option = new LinkedList();
        option.add(txt);
        original = txt;
        option_rep = txt + "+";
        superstring = new LinkedList();

        if (typ == 9) {
            option.add("");
            option_rep = option_rep + "Absent+";
        }
    }

    public String text() {
        return text;	
    }

    public void add_super(String s_str) {
        if (!superstring.contains(s_str)) {
            superstring.add(s_str);
        }
    }

    public String print_rep() {
        return option_rep;
    }

    public String get(int i) {
        return (String) (option.get(i));
    }

    //use option word to build to structure, one is a linkedlist and the other one is string saperate by op. --hubin
    public void add_option(String op) {
        option.add(op);              
        option_rep = option_rep + op + "+";
    }

    public int size() {
        return option.size();
    }
    
    public String toString(){
    	return "SUBTERMSUBTERM object:  text = " + text + ";;;;    the option words = " + option_rep + ";;;;    type = " + type;
    }
}

