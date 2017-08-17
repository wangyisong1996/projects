#ifndef LANGUAGE_H
#define LANGUAGE_H

class Language {
public:
	static int get_lang() {
		return lang;
	}
	
	static void set_lang(int l) {
		if (l != 0 && l != 1) return;
		lang = l;
	}

private:
	static int lang;
	// 0 -- English, 1 -- Chinese
};

#endif
