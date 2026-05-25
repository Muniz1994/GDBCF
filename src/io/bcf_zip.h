#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

// BCFZip: thin wrapper around miniz for reading and writing BCF container files.
// BCF files are standard ZIP archives renamed to .bcf.
class BCFZip {
public:
    BCFZip();
    ~BCFZip();

    // ── Read ─────────────────────────────────────────────────────────────────
    // Open a BCF file from disk. Returns true on success.
    bool open_read(const std::string &path);

    // Open from a raw memory buffer (e.g., downloaded bytes).
    bool open_read_buffer(const uint8_t *data, size_t size);

    // List all entry paths inside the archive (e.g. "bcf.version", "{guid}/markup.bcf").
    std::vector<std::string> list_entries() const;

    // Read a single entry by exact path. Returns empty vector if not found.
    std::vector<uint8_t> read_entry(const std::string &entry_path) const;

    // Convenience: read text entry as string. Returns "" if not found.
    std::string read_text_entry(const std::string &entry_path) const;

    // Check if an entry exists.
    bool has_entry(const std::string &entry_path) const;

    // ── Write ─────────────────────────────────────────────────────────────────
    // Start building a new archive in memory.
    void open_write();

    // Add or overwrite an entry with raw bytes.
    void add_entry(const std::string &entry_path, const std::vector<uint8_t> &data);

    // Convenience: add a text entry (UTF-8).
    void add_text_entry(const std::string &entry_path, const std::string &text);

    // Finalize the archive and return the raw .bcf bytes.
    std::vector<uint8_t> finalize();

    // Finalize and write directly to disk. Returns true on success.
    bool write_to_file(const std::string &path);

private:
    // Read: in-memory copy of the ZIP data so miniz can work on it.
    std::vector<uint8_t> m_read_buf;
    bool m_is_open_read = false;

    // Write: accumulated entries before finalization.
    bool m_is_open_write = false;
    std::vector<std::pair<std::string, std::vector<uint8_t>>> m_write_entries;
};
