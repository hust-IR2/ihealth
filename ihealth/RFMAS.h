#pragma once
#include <vector>
#include <tchar.h>
#include <map>

class RFMAS
{
public:
	RFMAS(void);
	~RFMAS(void);

	void init();

	void Next();
	void Prev();
	void Reset();
	int getScore();
	void saveAnswer(int i , int index);
	void getQuestionAndAnswer(std::vector<std::wstring>& questions, std::vector<std::wstring>& answers);
	std::wstring getQuestion(int i);
	std::vector<std::wstring> getAnswers(int i);

	int m_current_index;
	std::map<int, int> m_result;
	std::vector<std::vector<std::wstring>> m_answers;
	std::vector<std::wstring> m_questions;
};

class RFFMA
{
public:
	RFFMA(void);
	~RFFMA();

	void init();

	void Next();
	void Prev();
	void Reset();

	int getScore();
	void saveAnswer(int i , int index);
	void getQuestionAndAnswer(std::vector<std::wstring>& questions, std::vector<std::wstring>& answers);
	std::wstring getQuestion(int i);
	std::vector<std::wstring> getAnswers(int i);

	std::map<int, int> m_result;
	std::vector<std::vector<std::wstring>> m_answers;
	std::vector<std::wstring> m_questions;
	int m_current_index;
};

