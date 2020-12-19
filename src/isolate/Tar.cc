#include <archive.h>
#include <archive_entry.h>
#include <err.h>
#include <fcntl.h>
#include <fcntl.h>
#include <libtar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <memory>
#include <sstream>

#include <isolate/Tar.hh>

namespace isolate
{
    static int
    copy_data(struct archive *ar, struct archive *aw)
    {
        int r = ARCHIVE_OK;
        const void *buff;
        size_t size;
#if ARCHIVE_VERSION_NUMBER >= 3000000
        int64_t offset;
#else
        off_t offset;
#endif

        for (r = archive_read_data_block(ar, &buff, &size, &offset);
                r != ARCHIVE_EOF; r = archive_read_data_block(ar, &buff, &size, &offset))
        {
            if (r != ARCHIVE_OK)
                return (r);
            r = archive_write_data_block(aw, buff, size, offset);
            if (r != ARCHIVE_OK)
            {
                TarException t("could not write block", archive_error_string(aw));
                warnx(t.what());
                return (r);
            }
        }
        return ARCHIVE_OK;
    }

    Tar::Tar()
        : a_(archive_read_new()),
          ext_(archive_write_disk_new())
    {
        if (a_ == NULL || ext_ == NULL)
            throw TarException("could not create archive structure");

        archive_write_disk_set_options(ext_, 0);
        archive_read_support_format_tar(a_);
        archive_read_support_filter_gzip(a_);
    }

    Tar::~Tar()
    {
        archive_read_close(a_);
        archive_read_free(a_);
        archive_write_close(ext_);
        archive_write_free(ext_);
    }

    void Tar::untar(const std::string& data)
    {
        struct archive_entry *entry;
        int r;

        if ((r = archive_read_open_memory(a_, data.c_str(), data.size())))
            throw TarException("could not open tar", archive_error_string(a_));
        for (r = archive_read_next_header(a_, &entry);
                r != ARCHIVE_EOF; r = archive_read_next_header(a_, &entry))
        {
            if (r != ARCHIVE_OK)
                throw TarException("could not untar tar", archive_error_string(a_));

            r = archive_write_header(ext_, entry);
            if (r != ARCHIVE_OK)
            {
                TarException t("could not write header", archive_error_string(a_));
                warnx(t.what());
            }
            else
            {
                copy_data(a_, ext_);
                r = archive_write_finish_entry(ext_);
                if (r != ARCHIVE_OK)
                    throw TarException("could not untar", archive_error_string(a_));
            }
        }
    }


    TarException::TarException(const std::string& msg)
        : msg_(msg)
    {}

    TarException::TarException(const std::string& msg, const std::string& tar_error)
    {
        std::stringstream ss;
        ss << msg << ": " << tar_error;
        msg_ = ss.str();
    }

    const char* TarException::what()
    {
        return msg_.c_str();
    }
}
