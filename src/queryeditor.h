#pragma once

#include <QWidget>
#include <QSyntaxHighlighter>
#include "ui_queryeditor.h"
#include "query.h"
#include "table.h"

struct TSParser;
struct TSLanguage;
struct TSTree;

class QueryEditor;

class QueryHighlighter : public QSyntaxHighlighter {
public:
	QueryHighlighter(QueryEditor* qe) : 
		QSyntaxHighlighter((QObject*)qe), editor(qe) 
	{	
	}

	void highlightBlock(const QString& text) override;

protected:
	QueryEditor* editor;
};

class QueryEditor : public QWidget
{
	Q_OBJECT

public:
	QueryEditor(QWidget *parent = nullptr);
	~QueryEditor();

	TSParser* parser;
	TSTree* tree;
	const TSLanguage* lang;

	Table* table;
	std::string parse();

private:
	Ui::QueryEditorClass ui;
	QueryHighlighter* highlighter;

	
};
