// File: tests/buffer_demo.cpp
// C++98 - Démo + tests “réseau” pour libftpp::Buffer
//
// Dépendances :
// - pad_line(std::string) doit exister (fourni par ta libftpp)
// - class libftpp::Buffer (dans Buffer.hpp)

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

#include "../include/libftpp.hpp"
#include "../include/Buffer.hpp"

static void test_1_headers_crlfcrlf_basic(int& ok, int& ko);
static void test_2_readline_basic_and_empty_line(int& ok, int& ko);
static void test_3_overflow_refuse_append(int& ok, int& ko);
static void test_4_take_peek_find_consume(int& ok, int& ko);
static void test_5_consume_compaction_coherence(int& ok, int& ko);

static void test_6_split_delimiters_across_chunks(int& ok, int& ko);
static void test_7_binary_safety_with_nul(int& ok, int& ko);
static void test_8_setmax_and_resetoverflow(int& ok, int& ko);
static void test_9_edge_cases_empty_and_take_zero(int& ok, int& ko);
static void test_10_find_with_offset_window(int& ok, int& ko);

int main() {
    int ok = 0;
    int ko = 0;

    std::cout << pad_line("Buffer demo / tests (C++98)") << "\n\n";

    // Garde tes 5 tests initiaux
    test_1_headers_crlfcrlf_basic(ok, ko);
    test_2_readline_basic_and_empty_line(ok, ko);
    test_3_overflow_refuse_append(ok, ko);
    test_4_take_peek_find_consume(ok, ko);
    test_5_consume_compaction_coherence(ok, ko);

    // Ajouts “plus sérieux”
    test_6_split_delimiters_across_chunks(ok, ko);
    test_7_binary_safety_with_nul(ok, ko);
    test_8_setmax_and_resetoverflow(ok, ko);
    test_9_edge_cases_empty_and_take_zero(ok, ko);
    test_10_find_with_offset_window(ok, ko);

    std::cout << "\n" << pad_line("Résumé") << "\n";
    std::cout << "OK: " << ok << "\n";
    std::cout << "KO: " << ko << "\n\n";

    return (ko == 0) ? 0 : 1;
}

// ===================== Helpers (statics) =====================

static void title(const std::string& t) {
    std::cout << pad_line(t) << "\n";
}

static void expect_(bool cond, const std::string& msg, int& ok, int& ko) {
    if (cond) {
        ++ok;
        std::cout << "  [OK] " << msg << "\n";
    } else {
        ++ko;
        std::cout << "  [KO] " << msg << "\n";
    }
}

static std::string to_hex(const std::string& s) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        oss << std::setw(2) << (unsigned int)c;
        if (i + 1 != s.size()) oss << " ";
    }
    return oss.str();
}

static void dump_window(const libftpp::Buffer& b, const std::string& label) {
    std::string all;
    b.peek(b.size(), all);
    std::cout << "  " << label << " size=" << b.size()
              << " max=" << b.max()
              << " overflow=" << (b.overflow() ? "true" : "false") << "\n";
    // On affiche aussi en “hex” pour éviter les soucis si binaire.
    std::cout << "  window(hex): " << to_hex(all) << "\n";
}

static void append_chunk(libftpp::Buffer& b, const std::string& chunk, const std::string& label) {
    b.append(chunk);
    std::cout << "  append " << label << " (" << chunk.size() << " bytes)\n";
}

// 1) Headers en morceaux + readUntilCRLFCRLF
static void test_1_headers_crlfcrlf_basic(int& ok, int& ko) {
    title("1) Headers en morceaux (CRLFCRLF) + reste");
    libftpp::Buffer b(64 * 1024);

    append_chunk(b, "GET / HTTP/1.1\r\nHost: ex", "#1");
    append_chunk(b, "ample.com\r\nUser-Agent: X\r\n\r\nBODY", "#2");

    std::string headers;
    bool got = b.readUntilCRLFCRLF(headers);

    expect_(got, "readUntilCRLFCRLF retourne true", ok, ko);
    expect_(headers.find("GET / HTTP/1.1") != std::string::npos, "headers contient la request-line", ok, ko);
    expect_(headers.find("Host: example.com") != std::string::npos, "headers contient Host complet", ok, ko);

    std::string rest;
    b.peek(b.size(), rest);
    expect_(rest == "BODY", "le buffer conserve le reste (BODY)", ok, ko);
    std::cout << "\n";
}

// 2) readLineCRLF : lignes + ligne vide
static void test_2_readline_basic_and_empty_line(int& ok, int& ko) {
    title("2) readLineCRLF (lignes) + ligne vide");
    libftpp::Buffer b(1024);

    b.append("A\r\nB\r\n\r\n");

    std::string line;
    bool a = b.readLineCRLF(line);
    expect_(a && line == "A", "1ère ligne = A", ok, ko);

    bool bb = b.readLineCRLF(line);
    expect_(bb && line == "B", "2ème ligne = B", ok, ko);

    bool empty = b.readLineCRLF(line);
    expect_(empty && line.empty(), "3ème ligne = vide (fin headers)", ok, ko);

    expect_(b.size() == 0, "après 3 readLineCRLF, fenêtre vide", ok, ko);
    std::cout << "\n";
}

// 3) overflow : refus d'append si dépasse max
static void test_3_overflow_refuse_append(int& ok, int& ko) {
    title("3) overflow : refus d'append au-delà de max");
    libftpp::Buffer b(10);

    b.append("12345678"); // 8 bytes
    expect_(b.size() == 8, "après append 8 bytes, size=8", ok, ko);
    expect_(!b.overflow(), "overflow=false avant dépassement", ok, ko);

    b.append("ABCDE"); // +5 => dépasserait 10
    expect_(b.overflow(), "overflow=true après tentative de dépassement", ok, ko);

    std::string win;
    b.peek(b.size(), win);
    expect_(win == "12345678", "contenu inchangé (append refusé)", ok, ko);
    std::cout << "\n";
}

// 4) take/peek/find/consume : cohérence générale
static void test_4_take_peek_find_consume(int& ok, int& ko) {
    title("4) take / peek / find / consume (cohérence)");
    libftpp::Buffer b(1024);
    b.append("HELLO\r\nWORLD\r\n\r\n");

    // find
    expect_(b.find("\r\n") == 5, "find(CRLF) retourne 5 sur 'HELLO\\r\\n...'", ok, ko);
    expect_(b.find("\r\n\r\n") == 12, "find(CRLFCRLF) retourne 12", ok, ko);

    // peek
    std::string out;
    bool p = b.peek(5, out);
    expect_(p && out == "HELLO", "peek(5) = HELLO", ok, ko);
    expect_(b.size() == std::string("HELLO\r\nWORLD\r\n\r\n").size(), "peek ne consomme pas", ok, ko);

    // take
    bool t = b.take(7, out); // "HELLO\r\n"
    expect_(t && out == "HELLO\r\n", "take(7) consomme HELLO+CRLF", ok, ko);

    // readline sur ce qui reste
    std::string line;
    bool rl = b.readLineCRLF(line);
    expect_(rl && line == "WORLD", "readLineCRLF lit WORLD", ok, ko);

    bool empty = b.readLineCRLF(line);
    expect_(empty && line.empty(), "ligne vide finale", ok, ko);

    expect_(b.size() == 0, "buffer vide à la fin", ok, ko);
    std::cout << "\n";
}

// 5) consume + compaction : test cohérence (sans introspection mémoire)
static void test_5_consume_compaction_coherence(int& ok, int& ko) {
    title("5) consume + compaction (cohérence taille & contenu)");
    libftpp::Buffer b(200000);

    std::string big(20000, 'X');
    b.append(big);
    expect_(b.size() == 20000, "append 20000 => size=20000", ok, ko);

    b.consume(15000);
    expect_(b.size() == 5000, "consume(15000) => size=5000", ok, ko);

    b.append("END");
    expect_(b.size() == 5003, "append END => size=5003", ok, ko);

    std::string tail;
    b.peek(b.size(), tail);
    expect_(tail.size() == 5003, "peek(size) retourne 5003 bytes", ok, ko);
    expect_(tail.substr(tail.size() - 3) == "END", "suffix = END", ok, ko);

    std::cout << "\n";
}

// 6) Délimiteurs split en plusieurs chunks (cas réseau réaliste)
static void test_6_split_delimiters_across_chunks(int& ok, int& ko) {
    title("6) Delimiters split (CRLF / CRLFCRLF en morceaux)");
    libftpp::Buffer b(4096);

    // Split CRLFCRLF: "\r\n\r\n" en 3 morceaux
    b.append("GET / HTTP/1.1\r\nHost: a");
    std::string headers;
    expect_(!b.readUntilCRLFCRLF(headers), "pas de CRLFCRLF (encore)", ok, ko);

    b.append("b\r");
    expect_(!b.readUntilCRLFCRLF(headers), "pas de CRLFCRLF (\\r seul)", ok, ko);

    b.append("\n\r\n");
    bool got = b.readUntilCRLFCRLF(headers);
    expect_(got, "CRLFCRLF détecté après les chunks", ok, ko);
    expect_(headers.find("Host: ab") != std::string::npos, "headers reconstitués correctement", ok, ko);

    // Split CRLF ligne: "Z\r\n" en 2 morceaux
    b.clear();
    b.append("Z\r");
    std::string line;
    expect_(!b.readLineCRLF(line), "readLineCRLF false si '\\r' seul", ok, ko);
    b.append("\n");
    expect_(b.readLineCRLF(line) && line == "Z", "readLineCRLF ok après '\\n'", ok, ko);

    std::cout << "\n";
}

// 7) Binaire-safe : présence de '\0' au milieu
static void test_7_binary_safety_with_nul(int& ok, int& ko) {
    title("7) Binaire-safe (presence de NUL dans le buffer)");
    libftpp::Buffer b(1024);

    // Construire bytes: 'A' '\0' 'B' '\0' 'C'
    const char raw[] = { 'A', '\0', 'B', '\0', 'C' };
    b.append(raw, 5);

    expect_(b.size() == 5, "append(raw,5) => size=5", ok, ko);

    std::string out;
    bool took = b.take(5, out);
    expect_(took, "take(5) réussit", ok, ko);
    expect_(out.size() == 5, "out.size() == 5 (ne coupe pas sur NUL)", ok, ko);
    expect_(to_hex(out) == "41 00 42 00 43", "contenu exact (hex) = 41 00 42 00 43", ok, ko);
    expect_(b.size() == 0, "buffer vide après take", ok, ko);

    std::cout << "\n";
}

// 8) setMax / resetOverflow : comportement attendu
static void test_8_setmax_and_resetoverflow(int& ok, int& ko) {
    title("8) setMax / resetOverflow (gestion limite)");
    libftpp::Buffer b(0); // illimité au départ
    b.append("1234567890"); // 10

    expect_(b.size() == 10, "size=10 après append", ok, ko);
    expect_(!b.overflow(), "overflow=false", ok, ko);

    b.setMax(5);
    // Selon la spec qu’on a posée : si size() > max => overflow = true
    expect_(b.max() == 5, "max() == 5 après setMax(5)", ok, ko);
    expect_(b.overflow(), "overflow devient true si max < size()", ok, ko);

    b.resetOverflow();
    expect_(!b.overflow(), "resetOverflow remet overflow à false", ok, ko);

    // Maintenant, avec max=5 et size=10, un append doit être refusé (et overflow true)
    b.append("X");
    expect_(!b.overflow(), "append refuse (overflow true) quand déjà trop grand", ok, ko);

    // clear doit reset overflow et vider
    b.clear();
    expect_(b.size() == 0, "clear() => size=0", ok, ko);
    expect_(!b.overflow(), "clear() => overflow reset", ok, ko);

    std::cout << "\n";
}

// 9) Cas limites : empty/data/take(0)/peek(0)
static void test_9_edge_cases_empty_and_take_zero(int& ok, int& ko) {
    title("9) Cas limites (empty, take(0), peek(0), data sur vide)");
    libftpp::Buffer b(128);

    expect_(b.empty(), "empty() true au départ", ok, ko);
    expect_(b.size() == 0, "size() == 0 au départ", ok, ko);

    // data() : selon ton choix, peut être NULL ou non-NULL. On accepte les 2,
    // mais on exige que size()==0 => on ne dereference pas.
    const char* p = b.data();
    expect_((p == NULL) || (p != NULL), "data() sur vide: NULL ou pointeur safe (les 2 OK)", ok, ko);

    std::string out = "should stay";
    bool pk = b.peek(0, out);
    expect_(pk && out.empty(), "peek(0) => true et out vide", ok, ko);

    out = "xx";
    bool tk = b.take(0, out);
    expect_(tk && out.empty(), "take(0) => true et out vide", ok, ko);

    // take trop grand
    b.append("AB");
    out = "keep?";
    bool fail = b.take(3, out);
    expect_(!fail, "take(n>size) => false", ok, ko);

    std::cout << "\n";
}

// 10) find avec offset : s'assurer que find ignore le préfixe consommé
static void test_10_find_with_offset_window(int& ok, int& ko) {
    title("10) find() respecte la fenêtre (offset)");
    libftpp::Buffer b(1024);

    b.append("XXXX\r\nYYYY\r\n");
    expect_(b.find("YYYY") == 6, "find('YYYY') == 6 dans la fenêtre initiale", ok, ko);

    // Consommer "XXXX\r\n" => la fenêtre commence sur "YYYY\r\n"
    b.consume(6);
    expect_(b.find("YYYY") == 0, "après consume, 'YYYY' est en position 0 de la fenêtre", ok, ko);
    expect_(b.find("XXXX") == std::string::npos, "après consume, 'XXXX' n'est plus trouvable", ok, ko);

    std::string line;
    bool rl = b.readLineCRLF(line);
    expect_(rl && line == "YYYY", "readLineCRLF lit YYYY après consume", ok, ko);

    std::cout << "\n";
}