#pragma once
#include <vector>

class Obj
{

};

class Doc
{
public:

    explicit Doc(FILE* out_file) :
        _jbig2_global_obj_id(0),
        _pages_obj_id(0),
        _out_file(out_file)
    {
    }

    void startDocument()
    {
        fputs("%PDF-1.5\n",
            _out_file);
        _xref.push_back(ftell(_out_file));
        fputs("1 0 obj\n"
            "<<"
            "/Type/Catalog"
            "/Pages 2 0 R"
            ">>"
            "\nendobj\n",
            _out_file);
        //reserve for /Pages
        _xref.push_back(-1);
        _pages_obj_id = _xref.size();
    }

    void addSymbolTable(const uint8_t* stream, const size_t stream_size)
    {
        _xref.push_back(ftell(_out_file));
        _jbig2_global_obj_id = _xref.size();
        fprintf(_out_file,
            "%zu 0 obj\n"
            "<<"
            "/Length %zu"
            ">>",
            _jbig2_global_obj_id,
            stream_size);
        printStream(stream, stream_size);
        fputs("\nendobj\n", _out_file);
    }


    void addImage(const uint32_t width, const uint32_t height,
        const uint32_t xres, const uint32_t yres,
        const uint8_t* stream, const size_t stream_size)
    {
        size_t image_obj_id = printImage(width, height, stream, stream_size);
        size_t contents_obj_id = printContents(width*72.f / xres, height*72.f / yres);
        printPage(width*72.f / xres, height*72.f / yres, contents_obj_id, image_obj_id);
    }

    void endDocument()
    {
        printPages();
        const long int startxref = printXref();
        printTrailer(startxref);
    }

private:
    size_t printImage(const uint32_t width, const uint32_t height, const uint8_t* stream, const size_t stream_size)
    {
        _xref.push_back(ftell(_out_file));
        size_t image_obj_id = _xref.size();
        fprintf(_out_file,
            "%zu 0 obj\n"
            "<<"
            "/Length %zu"
            "/Type/XObject"
            "/Subtype/Image"
            "/Height %u"
            "/Width %u"
            "/ColorSpace/DeviceGray"
            "/BitsPerComponent 1"
            "/Filter/JBIG2Decode"
            "/DecodeParms<</JBIG2Globals %zu 0 R>>"
            ">>",
            image_obj_id,
            stream_size,
            height,
            width,
            _jbig2_global_obj_id);
        printStream(stream, stream_size);
        fputs("\nendobj\n", _out_file);
        return image_obj_id;
    }


    size_t printContents(const float width, const float height)
    {
        _xref.push_back(ftell(_out_file));
        size_t contents_obj_id = _xref.size();
        char buffer[128] = { 0 };
        snprintf(buffer, sizeof(buffer), "q %f 0 0 %f 0 0 cm /Im1 Do Q", width, height);
        size_t length = strlen(buffer);
        fprintf(_out_file,
            "%zu 0 obj\n"
            "<</Length %zu"
            ">>"
            "stream\n"
            "%s"
            "\nendstream"
            "\nendobj\n",
            contents_obj_id,
            length,
            buffer);
        return contents_obj_id;
    }

    void printPage(const float width, const float height,
        const size_t contents_obj_id, const size_t image_obj_id)
    {
        _xref.push_back(ftell(_out_file));
        size_t page_obj_id = _xref.size();
        fprintf(_out_file,
            "%zu 0 obj\n"
            "<<"
            "/Type/Page"
            "/MediaBox[0 0 %f %f]"
            "/Contents %zu 0 R"
            "/Resources<</XObject<</Im1 %zu 0 R >>/ProcSet[/PDF /ImageB]>>"
            "/Parent %zu 0 R"
            ">>"
            "\nendobj\n",
            page_obj_id,
            width,
            height,
            contents_obj_id,
            image_obj_id,
            _pages_obj_id);
        _pageIds.push_back(page_obj_id);
    }


    void printStream(const uint8_t* stream, const size_t stream_size) const
    {
        fputs("stream\n", _out_file);
        fwrite(stream, sizeof(uint8_t), stream_size, _out_file);
        fputs("\nendstream", _out_file);
    }

    void printPages()
    {
        _xref[1] = ftell(_out_file);
        fprintf(_out_file,
            "2 0 obj\n"
            "<<"
            "/Type/Pages"
            "/Count %zu"
            "/Kids[",
            _pageIds.size());
        for (auto page_id : _pageIds)
        {
            fprintf(_out_file, "%d 0 R ", page_id);
        }
        fputs("]"
            ">>"
            "\nendobj\n",
            _out_file);
    }

    long int printXref()
    {
        const long int startxref = ftell(_out_file);
        fprintf(_out_file,
            "xref\n"
            "0 %zu\n"
            "0000000000 65535 f \n",
            _xref.size() + 1);
        for (auto offset : _xref)
        {
            fprintf(_out_file, "%010d 00000 n \n", offset);
        }
        return startxref;
    }

    void printTrailer(const long int startxref)
    {
        fprintf(_out_file,
            "trailer\n"
            "<<"
            "/Size %zu"
            "/Root 1 0 R"
            ">>"
            "\nstartxref\n"
            "%d\n"
            "%%%%EOF",
            _xref.size() + 1,
            startxref);
    }

    size_t					_jbig2_global_obj_id;
    size_t					_pages_obj_id;
    std::vector<int>		_pageIds;
    std::vector<long int>	_xref;
    FILE* const				_out_file;
};