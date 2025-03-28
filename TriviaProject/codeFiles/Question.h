#pragma once
#include <string>
#include <vector>


#define ID_OF_THE_ANSWER_THAT_ALLWAYS_CORRECT 1


class Question {
public:

	// setters
	void setQuestion(std::string question);
	void setPossibleAnswers(std::vector<std::string> possibleAnswers);

	// getters
	std::string getQuestion();
	std::vector<std::string> getPossibleAnswers();
	int getCorrectAnswerId(); 
	// CORRECT ANSWER IS ALWAYS ID = 1, THERE ARE 4 ID'S FOR 4 OPTIONS. WE WILL RANDOMIZE THE ORDER IN CLIENT CODE

private:
	std::string m_question;
	std::vector<std::string> m_possibleAnswers;
};