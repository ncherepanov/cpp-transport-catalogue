#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <set>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"

namespace inputspace{

struct CommandDescription {
    // Определяет, задана ли команда (поле command непустое)
    explicit operator bool() const {
        return !command.empty();
    }

    bool operator!() const {
        return !operator bool();
    }

    std::string command;      // Название команды
    std::string id;           // id маршрута или остановки
    std::string description;  // Параметры команды
};

class InputReader {
public:
    /**
     * Парсит строку в структуру CommandDescription и сохраняет результат в commands_
     */
    void ParseLine(std::string_view line);
    
    const std::vector<CommandDescription>& GetCommands() {
        return commands_;
    }

    /**
     * Наполняет данными транспортный справочник, используя команды из commands_
     */
    void ApplyCommands(catalogue::TransportCatalogue& catalogue) const;
    
    void AddToCatalogue(std::istream& in, catalogue::TransportCatalogue& catalogue);

private:
    std::vector<CommandDescription> commands_;
};

}