#ifndef KVMAP_HEADER_
#define KVMAP_HEADER_

#include <map>
#include <string>

#include <boost/lexical_cast.hpp>

class KVMap {
public:
	typedef std::map<std::string, std::string> container_type;
	typedef container_type::iterator iterator;
	typedef container_type::const_iterator const_iterator;

public:
	iterator begin() { return c_.begin(); }
	iterator end() { return c_.end(); }
	const_iterator begin() const { return c_.begin(); }
	const_iterator end() const { return c_.end(); }

	template<typename T>
	bool lookupValue(T& value, const std::string& key) const
	{
		auto it = c_.find(key);
		if (it != c_.end()) {
			try {
				value = boost::lexical_cast<T>(it->second);
				return true;
			}
			catch (...) {
			}
		}
		return false;
	}

	std::string& operator[](const std::string& key)
	{
		return c_[key];
	}

	bool has(const std::string& key) const
	{
		return (c_.find(key) != c_.end());
	}

	std::string& at(const std::string& key)
	{
		return c_.at(key);
	}

	const std::string& at(const std::string& key) const
	{
		return c_.at(key);
	}

	const std::string& operator()(const std::string& key) const
	{
		auto it = c_.find(key);
		if (it != c_.end()) {
			return it->second;
		}
		return empty_;
	}

private:
	container_type c_;
	std::string empty_;
};


#endif // KVMAP_HEADER_

