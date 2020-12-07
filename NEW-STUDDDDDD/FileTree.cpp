#include <fstream>
#include <iostream>
#include "FileTree.h"
#define LOG(x) std::cout << x << std::endl


// Tag -------------------------------------------------------
Tag::Tag(std::string tag_string, AttribList* attribs) {

	this->tag_string = tag_string;

	// make name
	std::size_t end = tag_string.find("/>");
	if (end == std::string::npos) {
		end = this->tag_string.length() - 1;
	}

	for (int i = 1; i < end; i++) {
		if (this->tag_string[i] == ' ') {
			break;
		}
		name += this->tag_string[i];
	}

	if (tag_string.find("/>") == std::string::npos) {
		end_tag = "</" + name + ">";
	}
	
	this->attribs = attribs;
	this->parent = parent;
}

std::string Tag::get_name() {
	return name;
}

std::string Tag::get_content() {
	return content;
}

std::string Tag::get_attrib_string() {
	std::string attrib_string;
	AttribList* next_elem = attribs;
	while (next_elem != nullptr) {
		attrib_string += next_elem->get_attrib()->key + ": " + next_elem->get_attrib()->value + "; ";
		next_elem = next_elem->next_attrib();
	}
	return attrib_string;
}

std::string Tag::get_end_tag() {
	return end_tag;
}

Tag* Tag::get_parent() {
	return parent;
}

void Tag::set_content(std::string content) {
	this->content = content;
}

void Tag::init(std::string* filestring, std::size_t* string_pos) {
	// check for content
	std::size_t content_end = filestring->find('<', *string_pos);

	while (*string_pos < filestring->length()) {
		// check for content
		for (; *string_pos < content_end; (*string_pos)++) {
			content += (*filestring)[*string_pos];
		}

		// generate child
		Tag* child = make_new_tag(filestring, string_pos);
		if (child) {
			child->init(filestring, string_pos);

			if (children) {
				children->append(child);
			}
			else {
				children = new TagList(child);
			}
		}
		else {
			*string_pos = filestring->find('<', *string_pos);
		}
	}

	LOG(this->name + " has content: " + this->content);
}


// FileTree --------------------------------------------------
FileTree::FileTree(std::string filepath) {
	this->filepath = filepath;
}

Tag* FileTree::get_root() {
	return root_tag;
}


void FileTree::parse() {
	std::ifstream file;
	std::string filestring;

	file.open(filepath);

	// write file into one string
	while (file.good()) {
		filestring += file.get();
	}

	file.close();

	// find Doctype
	std::size_t string_pos = filestring.find("<?");

	if (string_pos != std::string::npos) {
		LOG("Doctype found at pos:");
		LOG(string_pos);
		string_pos++;
		string_pos = filestring.find("<", string_pos);
	}
	else {
		string_pos = 0;
	}
	// make root name
	
	root_tag = make_new_tag(&filestring, &string_pos);
	
	//string_pos++;
	//end = filestring.find(root_tag->get_end_tag(), string_pos);
	root_tag->init(&filestring, &string_pos);
}

// TagList -----------------------------------------------------
Tag::TagList::TagList(Tag* tag) {
	this->tag = tag;
}

Tag* Tag::TagList::get_tag() {
	return tag;
}

Tag::TagList* Tag::TagList::next_tag() {
	return next;
}

void Tag::TagList::append(Tag* next_tag) {
	if (!next) {
		TagList* new_next = new TagList(next_tag);
		next = new_next;
	}
	else {
		next->append(next_tag);
	}
}

// AttribList -----------------------------------------------------

AttribList::AttribList(Attrib* attrib) {
	this->attrib = attrib;
}

Attrib* AttribList::get_attrib() {
	return attrib;
}

AttribList* AttribList::next_attrib() {
	return next;
}

void AttribList::append(Attrib* next_attrib) {
	if (!next) {
		AttribList* new_next = new AttribList(next_attrib);
		next = new_next;
	}
	else {
		next->append(next_attrib);
	}
}

// Fuctions
AttribList* make_attrib_list(std::string* filestring) {
	std::size_t string_pos = filestring->find(' ');
	std::size_t attribs_end = filestring->find_last_of('"');
	AttribList* attribs = nullptr;
	if (string_pos != std::string::npos) {
		for (string_pos++; string_pos < attribs_end; (string_pos)++) {
			std::string attrib_key, attrib_value;
			
			// make attrib key
			for (; string_pos < filestring->find('=', string_pos); (string_pos)++) {
				attrib_key += (*filestring)[string_pos];
			}

			string_pos = filestring->find('"', string_pos);
			string_pos++;
			std::size_t end_value = filestring->find('"', string_pos);
			// make attrib value
			for (string_pos; string_pos < end_value; string_pos++) {
				attrib_value += (*filestring)[string_pos];
			}

			string_pos++;

			// make attrib
			Attrib* attrib = new Attrib();
			attrib->key = attrib_key;
			attrib->value = attrib_value;

			// check if attribs has a header
			if (!attribs) {
				LOG("Generate AttribList header!");
				attribs = new AttribList(attrib);
			}
			else {
				attribs->append(attrib);
			}
		}
	}
	return attribs;
}

Tag* make_new_tag(std::string* filestring, std::size_t* string_pos) {
	Tag* tag = nullptr;

	// make tag_string
	std::string tag_string;
	std::size_t tag_end = filestring->find('>', *string_pos);
	for (; *string_pos <= tag_end; (*string_pos)++) {
		tag_string += (*filestring)[*string_pos];
	}

	if (tag_string.find("</") == std::string::npos) {
		// make AttribList
		AttribList* attribs = make_attrib_list(&tag_string);

		//make tag
		tag = new Tag(tag_string, attribs);

		LOG("Tag " + tag->get_name() + " generated");
		LOG(tag->get_name() + " attributes: " + tag->get_attrib_string());
	}
	
	return tag;
}