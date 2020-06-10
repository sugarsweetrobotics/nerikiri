#pragma once

#include <string>

namespace nerikiri {

    /**
     * 文字列内の特定文字列の置き換え
     * @param ss 置き換え元
     * @param p パターン
     * @param m 置き換え文字列
     * @return 置き換え後の文字列
     */
    std::string replaceAll(const std::string ss, const std::string& p, const std::string& m) {
        auto s = ss; // コピーする
        auto pos = s.find(p);
        int toLen = m.length();
    
        // もしパターンが空の文字列ならば何もしない
        if (p.empty()) { return s;}
    
        while ((pos = s.find(p, pos)) != std::string::npos) {
            s.replace(pos, p.length(), m);
            pos += toLen;
        }
        return s;
    }

}