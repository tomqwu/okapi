
import java.util.*;

public class Concept_words_LinkedList {
public LinkedList concept_words_linkedlist;

	Concept_words_LinkedList(){
		concept_words_linkedlist = new LinkedList();
	}
	
	public Concept_words_LinkedList add(String op_text){
		concept_words_linkedlist.add(op_text);
		return this;
	}
}

