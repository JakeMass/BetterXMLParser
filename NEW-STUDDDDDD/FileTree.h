#pragma once
#include <string>
#include <fstream>

class Attrib {
public:
	std::string key, value;
};

class AttribList {
private:
	Attrib* attrib = nullptr;
	AttribList* next = nullptr;

public:
	AttribList() {}
	AttribList(Attrib* attrib);
	Attrib* get_attrib();
	AttribList* next_attrib();
	void append(Attrib* next_attrib);
};

class Tag {
private:

	class TagList {
	private:
		Tag* tag = nullptr;
		TagList* next = nullptr;

	public:
		TagList() {}
		TagList(Tag* tag);
		Tag* get_tag();
		TagList* next_tag();
		void append(Tag* next_tag);
	};

	std::string name;
	std::string content;
	std::string end_tag = "";
	AttribList* attribs;
	TagList* children;
	Tag* parent;

public:
	Tag(){}
	Tag(std::string tag_string, AttribList* attribs);
	std::string tag_string;
	std::string get_name();
	std::string get_content();
	std::string get_attrib_string();
	std::string get_end_tag();
	Tag* get_parent();
	void set_content(std::string content);
	void init(std::string* filestring, std::size_t* string_pos);
};

class FileTree {
private:
	Tag* root_tag;
	std::string filepath;

public:
	FileTree(std::string filepath);
	Tag* get_root();
	void parse();
};

AttribList* make_attrib_list(std::string* filestring);

Tag* make_new_tag(std::string* filestring, std::size_t* string_pos);
