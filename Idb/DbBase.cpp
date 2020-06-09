#include "DbBase.h"

Document DbBase::retrieve(string tablename, Document params, vector<string> fields)
{
	Document obj;
	obj.Parse("{}");
	Value rs("200");
	obj.AddMember("code", rs, obj.GetAllocator());
	return obj;
}
