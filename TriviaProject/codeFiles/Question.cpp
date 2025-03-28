#include "Question.h"

void Question::setQuestion(std::string question) {
	this->m_question = question;
}

void Question::setPossibleAnswers(std::vector<std::string> possibleAnswers) {
	// Go over the elements of possibleAnswers and push them back into m_possibleAnswers
	for (const auto& answer : possibleAnswers) {
		this->m_possibleAnswers.push_back(answer);
	}
}


std::string Question::getQuestion() {
	return this->m_question;
}

std::vector<std::string> Question::getPossibleAnswers() {
	return this->m_possibleAnswers;
}

int Question::getCorrectAnswerId() {
	return ID_OF_THE_ANSWER_THAT_ALLWAYS_CORRECT;
}