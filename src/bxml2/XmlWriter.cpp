// File: bxml2/XmlWriter.cpp

#include <gw2formats/bxml2/XmlWriter.h>

#include <algorithm>
#include <fstream>
#include <sstream>
#include <codecvt>

namespace gw2f {
namespace bxml2 {

namespace
{

bool outputElement(const XmlElement& p_element, std::wostream& p_stream, uint32_t p_depth)
{
    // start the starting tag
    std::wstring indent(p_depth, '\t');
    p_stream << indent << "<" << p_element.name();

    // attributes
    for (uint32_t i = 0; i < p_element.attributeCount(); i++) {
        p_stream 
            << " " 
            << p_element.attributes()[i]->name() 
            << "=\""
            << p_element.attributes()[i]->value()
            << "\"";
    }

    // close the starting tag
    if (!p_element.childCount())
    {
	    p_stream << ">" << p_element.value() << "<" << "/" << p_element.name();
    }
    p_stream << ">" << std::endl;

    // output children
    for (auto child = p_element.firstChild(); child; child = child->nextSibling()) {
        if (!outputElement(*child, p_stream, p_depth + 1)) {
            return false;
        }
    }

    // output end tag
    if (p_element.childCount()) {
		//output value
		if (p_element.value() != L"") p_stream << p_element.value();
        p_stream << indent << "</" << p_element.name() << ">" << std::endl;
    }

    return true;
}

}; // anon namespace

XmlWriter::XmlWriter()
{
}

XmlWriter::~XmlWriter()
{
}

bool XmlWriter::write(const XmlDocument& p_document, std::wostream& p_stream)
{
    p_stream << "<?xml version=\"" << p_document.version() << "\" encoding=\"" << p_document.encoding() << "\"?>" << std::endl;
    return outputElement(p_document.root(), p_stream, 0);
}

bool XmlWriter::write(const XmlDocument& p_document, const std::wstring& p_filename)
{
    std::wofstream output(p_filename, std::ios::out | std::ios::binary);
    if (!output.is_open()) { return false; }
	output.imbue(std::locale(output.getloc(), new std::codecvt_utf8_utf16<wchar_t>));
    auto retval = write(p_document, output);
    output.close();
    return retval;
}

uint32_t XmlWriter::write(const XmlDocument& p_document, byte* p_buffer, uint32_t p_maxSize)
{
    std::wstringstream stream;
    write(p_document, stream);

    auto size = std::min(static_cast<uint32_t>(stream.tellg()), p_maxSize);
    stream.seekg(0, std::ios::beg);
    stream.read(reinterpret_cast<wchar_t*>(p_buffer), size);

    return size;
}

}; // namespace bxml2
}; // namespace gw2f
