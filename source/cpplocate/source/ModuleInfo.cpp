
#include <cpplocate/ModuleInfo.h>

#include <iostream>
#include <fstream>

#include <cpplocate/utils.h>


namespace cpplocate
{


ModuleInfo::ModuleInfo()
{
}

ModuleInfo::ModuleInfo(const ModuleInfo & rh)
{
    m_values = rh.m_values;
}

ModuleInfo::~ModuleInfo()
{
}

ModuleInfo & ModuleInfo::operator=(const ModuleInfo & rh)
{
    clear();

    m_values = rh.m_values;
    return *this;
}

bool ModuleInfo::load(const std::string & filename)
{
    clear();

    std::string modulePath = utils::getDirectoryPath(filename);

    std::ifstream in(filename);
    if (in.is_open())
    {
        std::string line;
        while (std::getline(in, line))
        {
            size_t pos = line.find(":");
            if (pos == std::string::npos) {
                continue;
            }

            std::string key = line.substr(0, pos);
            utils::trim(key);

            std::string value = line.substr(pos+1);
            utils::trim(value);

            utils::replace(value, "${ModulePath}", modulePath);

            setValue(key, value);
        }

        in.close();

        return true;
    }

    return false;
}

bool ModuleInfo::save(const std::string & filename) const
{
    std::ofstream out(filename);
    if (out.is_open())
    {
        for (auto item : m_values)
        {
            out << item.first << ": " << item.second << std::endl;
        }
        out.close();

        return true;
    }

    return false;
}

void ModuleInfo::clear()
{
    m_values.clear();
}

bool ModuleInfo::empty() const
{
    return m_values.empty();
}

const std::map<std::string, std::string> & ModuleInfo::values() const
{
    return m_values;
}

const std::string & ModuleInfo::value(const std::string & key, const std::string & defaultValue) const
{
    if (m_values.count(key) > 0)
    {
        return m_values.at(key);
    }

    return defaultValue;
}

void ModuleInfo::setValue(const std::string & key, const std::string & value)
{
    m_values[key] = value;
}

void ModuleInfo::print() const
{
    for (const auto & item : m_values)
    {
        std::cout << item.first << ": " << item.second << std::endl;
    }
}


} // namespace cpplocate
