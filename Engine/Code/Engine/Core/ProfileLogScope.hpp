#pragma once


class ProfileLogScope
	{
	public:
		ProfileLogScope(const char*scopeName);
		~ProfileLogScope();

	public:
		const char* m_scopeName;
	};