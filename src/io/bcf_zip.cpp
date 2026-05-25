#include "bcf_zip.h"
#include "miniz.h"

#include <fstream>
#include <iterator>
#include <cstring>

BCFZip::BCFZip() = default;
BCFZip::~BCFZip() = default;

// ── Helpers ───────────────────────────────────────────────────────────────────

static std::vector<uint8_t> read_file_bytes(const std::string &path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return {};
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(f),
                                std::istreambuf_iterator<char>());
}

static bool write_file_bytes(const std::string &path, const std::vector<uint8_t> &data) {
    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    if (!f) return false;
    f.write(reinterpret_cast<const char *>(data.data()),
            static_cast<std::streamsize>(data.size()));
    return f.good();
}

// ── Read ──────────────────────────────────────────────────────────────────────

bool BCFZip::open_read(const std::string &path) {
    m_read_buf = read_file_bytes(path);
    if (m_read_buf.empty()) return false;
    m_is_open_read = true;
    return true;
}

bool BCFZip::open_read_buffer(const uint8_t *data, size_t size) {
    if (!data || size == 0) return false;
    m_read_buf.assign(data, data + size);
    m_is_open_read = true;
    return true;
}

std::vector<std::string> BCFZip::list_entries() const {
    if (!m_is_open_read) return {};

    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));
    if (!mz_zip_reader_init_mem(&zip, m_read_buf.data(), m_read_buf.size(), 0))
        return {};

    mz_uint count = mz_zip_reader_get_num_files(&zip);
    std::vector<std::string> result;
    result.reserve(count);

    for (mz_uint i = 0; i < count; i++) {
        mz_zip_archive_file_stat stat;
        if (mz_zip_reader_file_stat(&zip, i, &stat) && !stat.m_is_directory) {
            result.emplace_back(stat.m_filename);
        }
    }
    mz_zip_reader_end(&zip);
    return result;
}

std::vector<uint8_t> BCFZip::read_entry(const std::string &entry_path) const {
    if (!m_is_open_read) return {};

    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));
    if (!mz_zip_reader_init_mem(&zip, m_read_buf.data(), m_read_buf.size(), 0))
        return {};

    int idx = mz_zip_reader_locate_file(&zip, entry_path.c_str(), nullptr, 0);
    if (idx < 0) {
        mz_zip_reader_end(&zip);
        return {};
    }

    size_t out_size = 0;
    void *data = mz_zip_reader_extract_to_heap(&zip, static_cast<mz_uint>(idx),
                                               &out_size, 0);
    mz_zip_reader_end(&zip);

    if (!data) return {};
    std::vector<uint8_t> result(static_cast<uint8_t *>(data),
                                static_cast<uint8_t *>(data) + out_size);
    mz_free(data);
    return result;
}

std::string BCFZip::read_text_entry(const std::string &entry_path) const {
    auto bytes = read_entry(entry_path);
    if (bytes.empty()) return {};
    return std::string(reinterpret_cast<const char *>(bytes.data()), bytes.size());
}

bool BCFZip::has_entry(const std::string &entry_path) const {
    if (!m_is_open_read) return false;
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));
    if (!mz_zip_reader_init_mem(&zip, m_read_buf.data(), m_read_buf.size(), 0))
        return false;
    int idx = mz_zip_reader_locate_file(&zip, entry_path.c_str(), nullptr, 0);
    mz_zip_reader_end(&zip);
    return idx >= 0;
}

// ── Write ─────────────────────────────────────────────────────────────────────

void BCFZip::open_write() {
    m_write_entries.clear();
    m_is_open_write = true;
}

void BCFZip::add_entry(const std::string &entry_path, const std::vector<uint8_t> &data) {
    // Replace existing entry if present.
    for (auto &e : m_write_entries) {
        if (e.first == entry_path) {
            e.second = data;
            return;
        }
    }
    m_write_entries.emplace_back(entry_path, data);
}

void BCFZip::add_text_entry(const std::string &entry_path, const std::string &text) {
    add_entry(entry_path,
              std::vector<uint8_t>(
                  reinterpret_cast<const uint8_t *>(text.data()),
                  reinterpret_cast<const uint8_t *>(text.data()) + text.size()));
}

std::vector<uint8_t> BCFZip::finalize() {
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));
    if (!mz_zip_writer_init_heap(&zip, 0, 0)) return {};

    for (const auto &entry : m_write_entries) {
        if (!mz_zip_writer_add_mem(&zip, entry.first.c_str(),
                                   entry.second.data(), entry.second.size(),
                                   MZ_DEFAULT_COMPRESSION)) {
            mz_zip_writer_end(&zip);
            return {};
        }
    }

    void *buf = nullptr;
    size_t buf_size = 0;
    if (!mz_zip_writer_finalize_heap_archive(&zip, &buf, &buf_size)) {
        mz_zip_writer_end(&zip);
        return {};
    }
    mz_zip_writer_end(&zip);

    std::vector<uint8_t> result(static_cast<uint8_t *>(buf),
                                static_cast<uint8_t *>(buf) + buf_size);
    mz_free(buf);
    return result;
}

bool BCFZip::write_to_file(const std::string &path) {
    auto data = finalize();
    if (data.empty()) return false;
    return write_file_bytes(path, data);
}
