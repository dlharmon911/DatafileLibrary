#include <iostream>
#include <unordered_map>
#include <functional>
#include <stack>
#include <sstream>
#include <string>
#include <allegro5\allegro5.h>
#include "datafile/d_string.h"
#include "datafile/d_dson.h"

namespace dlh
{
	using file_t = ALLEGRO_FILE;

	dson_t::dson_t() = default;

	void dson_t::set_string(const std::string& sString, const size_t nItem)
	{
		if (nItem >= m_vContent.size())
			m_vContent.resize(nItem + 1);

		m_vContent[nItem] = sString;
	}

	// Retrieves the String Value of a Property (for a given index) or ""
	const std::string dson_t::get_string(const size_t nItem) const
	{
		if (nItem >= m_vContent.size())
			return "";
		else
			return m_vContent[nItem];
	}

	template <> int32_t dson_t::get_as(const size_t nItem) const
	{
		return std::atoi(this->get_string(nItem).c_str());
	}

	template <> double dson_t::get_as(const size_t nItem) const
	{
		return std::atof(this->get_string(nItem).c_str());
	}

	template <> bool dson_t::get_as(const size_t nItem) const
	{
		return dlh::string::to_lower(this->get_string(nItem)) == "true";
	}

	template <>
	void dson_t::set_as(const int32_t value, const size_t nItem)
	{
		this->set_string(std::to_string(value), nItem);
	}

	template <>
	void dson_t::set_as(const double value, const size_t nItem)
	{
		this->set_string(std::to_string(value), nItem);
	}

	template <>
	void dson_t::set_as(const bool value, const size_t nItem)
	{
		this->set_string(value ? "true" : "false", nItem);
	}

	//// Retrieves the Real Value of a Property (for a given index) or 0.0
	//const double dson_t::get_real(const size_t nItem) const
	//{
	//	return std::atof(get(nItem).c_str());
	//}

	//// Sets the Real Value of a Property (for a given index)
	//void dson_t::set_real(const double d, const size_t nItem)
	//{
	//	set_string(std::to_string(d), nItem);
	//}

	//// Retrieves the Integer Value of a Property (for a given index) or 0
	//const int32_t dson_t::get_integer(const size_t nItem) const
	//{
	//	return std::atoi(get(nItem).c_str());
	//}

	//// Sets the Integer Value of a Property (for a given index)
	//void dson_t::set_integer(const int32_t n, const size_t nItem)
	//{
	//	set_string(std::to_string(n), nItem);
	//}

	//// Retrieves the Boolean Value of a Property (for a given index) or 0
	//const int32_t dson_t::get_boolean(const size_t nItem) const
	//{
	//	return get(nItem) == "true";
	//}

	//// Sets the Boolean Value of a Property (for a given index)
	//void dson_t::set_boolean(const bool n, const size_t nItem)
	//{
	//	set_string(n ? "true" : "false", nItem);
	//}

	// Returns the number of Values a property consists of
	size_t dson_t::get_content_count() const
	{
		return m_vContent.size();
	}

	size_t dson_t::get_children_count() const
	{
		return this->m_mapObjects.size();
	}

	// Checks if a property exists - useful to avoid creating properties
	// via reading them, though non-essential
	bool dson_t::contains(const std::string& sName) const
	{
		return m_mapObjects.count(sName) > 0;
	}

	// Access a dson_t via a convenient name - "root.node.something.property"
	const dson_t& dson_t::get_property(const std::string& name) const
	{
		size_t x = name.find_first_of('.');
		if (x != std::string::npos)
		{
			std::string sProperty = name.substr(0, x);
			if (this->contains(sProperty))
				return operator[](sProperty).get_property(name.substr(x + 1, name.size()));
			else
				return operator[](sProperty);
		}
		else
		{
			return operator[](name);
		}
	}

	// Access a dson_t via a convenient name - "root.node.something.property"
	dson_t& dson_t::get_property(const std::string& name)
	{
		size_t x = name.find_first_of('.');
		if (x != std::string::npos)
		{
			std::string sProperty = name.substr(0, x);
			if (this->contains(sProperty))
				return operator[](sProperty).get_property(name.substr(x + 1, name.size()));
			else
				return operator[](sProperty);
		}
		else
		{
			return operator[](name);
		}
	}

	// Access a numbered element - "node[23]", or "root[56].node"
	dson_t& dson_t::get_indexed_property(const std::string& name, const size_t nIndex)
	{
		return get_property(name + "[" + std::to_string(nIndex) + "]");
	}

	bool dson_t::write(const dson_t& n, const std::string& sFileName, const std::string& sIndent, const char sListSep)
	{
		// Cache indentation level
		size_t nIndentCount = 0;
		// Cache sperator string for convenience
		std::string sSeperator = std::string(1, sListSep) + " ";

		// Fully specified lambda, because this lambda is recursive!
		std::function<void(const dson_t&, file_t*)> write = [&](const dson_t& n, file_t* pfile)
			{
				// Lambda creates string given indentation preferences
				auto indent = [&](const std::string& sString, const size_t nCount)
					{
						std::string sOut;
						for (size_t n = 0; n < nCount; n++) sOut += sString;
						return sOut;
					};

				// Iterate through each property of this node
				for (auto const& property : n.m_vecObjects)
				{
					// Does property contain any sub objects?
					if (property.second.m_vecObjects.empty())
					{
						// No, so it's an assigned field and should just be written. If the property
						// is flagged get_as comment, it has no assignment potential. First write the 
						// property name
						al_fputs(pfile, indent(sIndent, nIndentCount).c_str());
						al_fputs(pfile, property.first.c_str());
						al_fputs(pfile, (property.second.m_bIsComment ? "" : " = "));

						// Second, write the property value (or values, seperated by provided
						// separation charater
						size_t nItems = property.second.get_content_count();
						for (size_t i = 0; i < property.second.get_content_count(); i++)
						{
							// If the Value being written, in string form, contains the separation
							// character, then the value must be written inside quotation marks. Note, 
							// that if the Value is the last of a list of Values for a property, it is
							// not suffixed with the separator
							size_t x = property.second.get_string(i).find_first_of(sListSep);
							if (x != std::string::npos)
							{
								// Value contains separator, so wrap in quotes
								al_fputs(pfile, "\"");
								al_fputs(pfile, property.second.get_string(i).c_str());
								al_fputs(pfile, "\"");
								if (nItems > 1)
								{
									al_fputs(pfile, sSeperator.c_str());
								}
							}
							else
							{
								// Value does not contain separator, so just write out
								al_fputs(pfile, property.second.get_string(i).c_str());
								if (nItems > 1)
								{
									al_fputs(pfile, sSeperator.c_str());
								}
							}
							nItems--;
						}

						// Property written, move to next line
						al_fputs(pfile, "\n");
					}
					else
					{
						// Yes, property has properties of its own, so it's a node
						// Force a new line and write out the node's name
						al_fputs(pfile, indent(sIndent, nIndentCount).c_str());
						al_fputs(pfile, property.first.c_str());
						al_fputs(pfile, "\n");
						// Open braces, and update indentation
						al_fputs(pfile, indent(sIndent, nIndentCount).c_str());
						al_fputs(pfile, "{\n");
						nIndentCount++;
						// Recursively write that node
						write(property.second, pfile);
						// Node written, so close braces
						al_fputs(pfile, indent(sIndent, nIndentCount).c_str());
						al_fputs(pfile, "}\n");
					}
				}

				// We've finished writing out a node, regardless of state, our indentation
				// must decrease, unless we're top level
				if (nIndentCount > 0) nIndentCount--;
			};

		// Start Here! Open the file for writing
		file_t* file = al_fopen(sFileName.c_str(), "w");
		if (file)
		{
			// write the file starting form the supplied node
			write(n, file);
			return true;
		}
		return false;
	}

	bool dson_t::read(dson_t& n, const std::string& sFileName, const char sListSep)
	{
		const size_t max_buffer = 2048;
		int32_t line_number = 0;
		char buffer[max_buffer] = "";

		// Open the file!
		ALLEGRO_FILE* file = al_fopen(sFileName.c_str(), "rb");
		if (file)
		{
			// These variables are outside of the read loop, get_as we will
			// need to refer to previous iteration values in certain conditions
			std::string sPropName = "";
			std::string sPropValue = "";

			// The file is fundamentally structured get_as a stack, so we will read it
			// in a such, but note the data structure in memory is not explicitly
			// stored in a stack, but one is constructed implicitly via the nodes
			// owning other nodes (aka a tree)

			// I dont want to accidentally create copies all over the place, nor do 
			// I want to use pointer syntax, so being a bit different and stupidly
			// using std::reference_wrapper, so I can store references to dson_t
			// nodes in a std::container.
			std::stack<std::reference_wrapper<dson_t>> stkPath;
			stkPath.push(n);

			// read file line by line and process
			while (!al_feof(file))
			{
				// read line
				char* temp = al_fgets(file, buffer, max_buffer);
				if (!temp)
				{
					break;
				}
				std::string line = temp;

				// This little lambda removes whitespace from
				// beginning and end of supplied string
				auto trim = [](std::string& s)
					{
						s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
						s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
					};

				trim(line);

				// If line has content
				if (!line.empty())
				{
					// Test if its a comment...
					if (line[0] == '#')
					{
						// ...it is a comment, so ignore
						dson_t comment;
						comment.m_bIsComment = true;
						stkPath.top().get().m_vecObjects.push_back({ line, comment });
					}
					else
					{
						// ...it is content, so parse. Firstly, find if the line
						// contains an assignment. If it does then it's a property...
						size_t x = line.find_first_of('=');
						if (x != std::string::npos)
						{
							// ...so split up the property into a name, and its values!

							// Extract the property name, which is all characters up to
							// first assignment, trim any whitespace from ends
							sPropName = line.substr(0, x);
							trim(sPropName);

							// Extract the property value, which is all characters after
							// the first assignment operator, trim any whitespace from ends
							sPropValue = line.substr(x + 1, line.size());
							trim(sPropValue);

							// The value may be in list form: a, b, c, d, e, f etc and some of those
							// elements may exist in quotes a, b, c, "d, e", f. So we need to iterate
							// character by character and break up the value
							bool bInQuotes = false;
							std::string sToken;
							size_t nTokenCount = 0;
							for (const auto c : sPropValue)
							{
								// Is character a quote...
								if (c == '\"')
								{
									// ...yes, so toggle quote state
									bInQuotes = !bInQuotes;
								}
								else
								{
									// ...no, so proceed creating token. If we are in quote state
									// then just append characters until we exit quote state.
									if (bInQuotes)
									{
										sToken.append(1, c);
									}
									else
									{
										// Is the character our seperator? If it is
										if (c == sListSep)
										{
											// Clean up the token
											trim(sToken);
											// Add it to the vector of values for this property
											stkPath.top().get()[sPropName].set_string(sToken, nTokenCount);
											// Reset our token state
											sToken.clear();
											nTokenCount++;
										}
										else
										{
											// It isnt, so just append to token
											sToken.append(1, c);
										}
									}
								}
							}

							// Any residual characters at this point just make up the final token,
							// so clean it up and add it to the vector of values
							if (!sToken.empty())
							{
								trim(sToken);
								stkPath.top().get()[sPropName].set_string(sToken, nTokenCount);
							}
						}
						else
						{
							// ...but if it doesnt, then it's something structural
							if (line[0] == '{')
							{
								// Open brace, so push this node to stack, subsequent properties
								// will belong to the new node
								stkPath.push(stkPath.top().get()[sPropName]);
							}
							else
							{
								if (line[0] == '}')
								{
									// Close brace, so this node has been defined, pop it from the
									// stack
									stkPath.pop();
								}
								else
								{
									// Line is a property with no assignment. Who knows whether this is useful,
									// but we can simply add it get_as a valueless property...
									sPropName = line;
									// ...actually it is useful, get_as valuless properties are typically
									// going to be the names of new dson_t nodes on the next iteration
								}
							}
						}
					}
				}
				++line_number;
			}

			// Close and exit!
			al_fclose(file);
			return true;
		}

		// File not found, so fail
		return false;
	}

	const dson_t& dson_t::operator[](const std::string& name) const
	{
		auto a = m_mapObjects.find(name);
		return m_vecObjects[a->second].second;
	}

	dson_t& dson_t::operator[](const std::string& name)
	{
		// Check if this "node"'s map already contains an object with this name...
		if (m_mapObjects.count(name) == 0)
		{
			// ...it did not! So create this object in the map. First get a vector id 
			// and link it with the name in the unordered_map
			m_mapObjects[name] = m_vecObjects.size();
			// then creating the new, blank object in the vector of objects
			m_vecObjects.push_back({ name, dson_t() });
		}

		// ...it exists! so return the object, by getting its index from the map, and using that
		// index to look up a vector element.
		return m_vecObjects[m_mapObjects[name]].second;
	}


	dson_t::iterator dson_t::begin() { return iterator(*this, this->m_mapObjects.begin()); }
	dson_t::iterator dson_t::end() { return iterator(*this, this->m_mapObjects.end()); }
	dson_t::const_iterator dson_t::cbegin() const { return const_iterator(*this, this->m_mapObjects.cbegin()); }
	dson_t::const_iterator dson_t::cend() const { return const_iterator(*this, this->m_mapObjects.cend()); }

	bool dson_t::has_content() const { return this->m_vContent.size() > 0; }
	bool dson_t::has_children() const { return this->m_vecObjects.size() > 0; }

	void dson_t::clear()
	{
		this->m_vContent.clear();
		this->m_vecObjects.clear();
		this->m_mapObjects.clear();
		this->m_bIsComment = false;
	}

	bool dson_t::empty()
	{
		return this->m_vContent.empty() && this->m_vecObjects.empty();
	}

}
