class priority_query {
	int count;
	String passback;
	query query_list;

	priority_query() {
		count = -1;
		passback = "";
		query_list = null;
	}
	
	query getQueryList() {
		return query_list;
	}
	
	int getCount() {
		return count;
	}
	
	String getPassback() {
		return passback;
	}
	
	void setQueryList(query q) {
		query_list = q;
	}
	
	void setCount(int i) {
		count = i;
	}
	
	void setPassback(String p) {
		passback = p;
	}
}