#include "queryeditor.h"
#include <QMessageBox>
#include <string>
#include <variant>
#include <cstdint>

#include <tree_sitter/api.h>
#include "tree_sitter/tree-sitter-WDBx.h"

class ASTBuilder {
public:
	ASTBuilder(TSNode&& root, std::string&& doc) : 
		_root(root), _doc(doc)
	{
	}

	Query build() {
		if (ts_node_has_error(_root) || ts_node_is_null(_root)) {
			throw std::runtime_error("Query is invalid.");
		}

		std::string root_name = ts_node_type(_root);
		if (root_name != "source_file") {
			throw std::runtime_error("Invalid root.");
		}

		_cursor = ts_tree_cursor_new(_root);
		TSNode node = firstNamedChild();
		Query q;

		if (isNodeType(node, "expression")) {
			q.value = readExpression();
		}
		else {
			throw std::runtime_error("Expected expression.");
		}

		return q;
	}

protected:

	Expression readExpression() {
		Expression expr;
		TSNode node = firstNamedChild();

		if (!ts_node_is_null(node)) {
			std::string type_name = ts_node_type(node);

			if (type_name == "condition") {
				expr.value = readCondition();
			}
			else if (type_name == "condition_group") {
				expr.value = readConditionGroup();
			}
			else if (type_name == "expression") {
				expr = readExpression();
			}
			else {
				throw std::runtime_error("Expected condition or group.");
			}

			ts_tree_cursor_goto_parent(&_cursor);
			return expr;
		}

		throw std::runtime_error("Invalid expression.");
	}

	Condition readCondition() {
		Condition cond;

		{
			TSNode id_node = firstNamedChild();
			if (isNodeType(id_node, "identifier")) {
				cond.lhs = readIdentifer();
			}
			else {
				throw std::runtime_error("Expected lhs identifier.");
			}
		}

		{
			TSNode op_node = nextNamedSibling();
			if (isNodeType(op_node, "compare_op")) {
				cond.op = readCompareOp();
			}
			else {
				throw std::runtime_error("Expected compare op.");
			}
		}

		{
			TSNode rhs_node = nextNamedSibling();
			if (!ts_node_is_null(rhs_node)) {

				std::string type_name = ts_node_type(rhs_node);

				if (type_name == "number") {
					cond.rhs = readNumber();
				}
				else if(type_name == "string") {
					cond.rhs = readString();
				}
				else if (type_name == "array") {
					cond.rhs = readArray();
				}
				else if (type_name == "identifier") {
					cond.rhs = readIdentifer();
				}
				else {
					throw std::runtime_error("Unknown rhs.");
				}
			}
			else {
				throw std::runtime_error("Invalid rhs.");
			}
		}

		ts_tree_cursor_goto_parent(&_cursor);

		return cond;
	}

	ConditionGroup readConditionGroup() {
		ConditionGroup group;

		{
			TSNode lhs_node = firstNamedChild();
			if (isNodeType(lhs_node, "expression")) {
				group.lhs = std::make_unique<Expression>(readExpression());
			}
			else {
				throw std::runtime_error("Expected lhs expression.");
			}
		}
		
		{
			TSNode op_node = nextNamedSibling();
			if (isNodeType(op_node, "logic_op")) {
				group.op = readLogicOp();
			}
			else {
				throw std::runtime_error("Expected logic op.");
			}
		}

		{
			TSNode rhs_node = nextNamedSibling();
			if (isNodeType(rhs_node, "expression")) {
				group.rhs = std::make_unique<Expression>(readExpression());
			}
			else {
				throw std::runtime_error("Expected rhs expression.");
			}
		}

		ts_tree_cursor_goto_parent(&_cursor);

		return group;
	}

	LogicOperation readLogicOp() {
		TSNode node = ts_tree_cursor_current_node(&_cursor);

		auto view = nodeString(node);

		if (view == "AND") {
			return LogicOperation::AND;
		}
		else if (view == "OR") {
			return LogicOperation::OR;
		}

		throw std::runtime_error("Expected logic op value.");
	}

	CompareOperation readCompareOp() {
		TSNode node = ts_tree_cursor_current_node(&_cursor);

		auto view = nodeString(node);

		if (view == "==") {
			return CompareOperation::EQUAL;
		}
		else if (view == "!=") {
			return CompareOperation::NOT_EQUAL;
		}
		else if (view == "IN") {
			return CompareOperation::IN_ARR;
		}
		else if (view == "NOT IN") {
			return CompareOperation::NOT_IN_ARR;
		}
		else if (view == ">") {
			return CompareOperation::GREATER_THAN;
		}
		else if (view == ">=") {
			return CompareOperation::GREATER_THAN_EQUAL;
		}
		else if (view == "<") {
			return CompareOperation::LESS_THAN;
		}
		else if (view == "<=") {
			return CompareOperation::LESS_THAN_EQUAL;
		}

		throw std::runtime_error("Expected compare op value.");
	}

	Identifier readIdentifer() {
		Identifier id;

		{
			TSNode name_node = firstNamedChild();
			if (isNodeType(name_node, "id_name")) {
				id.value = nodeString(name_node);
			}
			else {
				throw std::runtime_error("Expected lhs expression.");
			}
		}

		{
			TSNode index_node = nextNamedSibling();
			if (isNodeType(index_node, "id_index")) {
				auto view = nodeString(index_node);
				uint64_t idx;
				if (std::from_chars(view.data(), view.data() + view.size(), idx).ec == std::error_code{}) {
					id.index = idx;
				}
				else {
					throw std::runtime_error("Invalid identifier index.");
				}
			}
		}

		ts_tree_cursor_goto_parent(&_cursor);

		return id;
	}

	String readString() {
		String str;

		TSNode val_node = firstNamedChild();
		str.value = nodeString(val_node);

		ts_tree_cursor_goto_parent(&_cursor);

		return str;
	}

	Number readNumber() {
		Number num;

		TSNode node = ts_tree_cursor_current_node(&_cursor);
		uint32_t count = ts_node_child_count(node);
		auto view = nodeString(node);

		if (count == 1) {
			// integer
			int64_t val;
			if (std::from_chars(view.data(), view.data() + view.size(), val).ec == std::error_code{}) {
				num.value = val;
			}
			else {
				throw std::runtime_error("Invalid number integer.");
			}
		}
		else {
			// floating point
			double val;
			if (std::from_chars(view.data(), view.data() + view.size(), val).ec == std::error_code{}) {
				num.value = val;
			}
			else {
				throw std::runtime_error("Invalid number float.");
			}

		}

		return num;
	}

	Array readArray() {
		Array arr;

		ts_tree_cursor_goto_first_child(&_cursor);
		TSNode node = ts_tree_cursor_current_node(&_cursor);

		while (!ts_node_is_null(node)) {
			if (ts_node_is_named(node)) {
				node = ts_tree_cursor_current_node(&_cursor);
				std::string type_name = ts_node_type(node);

				if (type_name == "number") {
					arr.push_back(readNumber());
				}
				else if (type_name == "string") {
					arr.push_back(readString());
				}
				else if (type_name == "identifier") {
					arr.push_back(readIdentifer());
				}
				else {
					throw std::runtime_error("Unknown rhs.");
				}
			}

			if (!ts_tree_cursor_goto_next_sibling(&_cursor)) {
				break;
			}
			node = ts_tree_cursor_current_node(&_cursor);
		}

		ts_tree_cursor_goto_parent(&_cursor);

		return arr;
	}

	bool isNodeType(TSNode node, const std::string& str) {
		if (!ts_node_is_null(node)) {
			std::string name = ts_node_type(node);
			return name == str;
		}
		return false;
	}

	TSNode firstNamedChild() {
		ts_tree_cursor_goto_first_child(&_cursor);
		TSNode node = ts_tree_cursor_current_node(&_cursor);
		std::string type_name = ts_node_type(node);
		
		while (!ts_node_is_named(node) && !ts_node_is_null(node)) {
			if (!ts_tree_cursor_goto_next_sibling(&_cursor)) {
				break;
			}
			node = ts_tree_cursor_current_node(&_cursor);
			type_name = ts_node_type(node);
		}
		
		return node;
	}

	TSNode nextNamedSibling() {
		ts_tree_cursor_goto_next_sibling(&_cursor);
		TSNode node = ts_tree_cursor_current_node(&_cursor);
		std::string type_name = ts_node_type(node);

		while (!ts_node_is_named(node) && !ts_node_is_null(node)) {
			if (!ts_tree_cursor_goto_next_sibling(&_cursor)) {
				break;
			}
			node = ts_tree_cursor_current_node(&_cursor);
			type_name = ts_node_type(node);
		}

		return node;
	}

	std::string_view nodeString(TSNode node) {
		auto start = ts_node_start_byte(node);
		auto end = ts_node_end_byte(node);
		auto view = std::string_view(_doc.data() + start, _doc.data() + end);
		return view;
	}


	TSNode _root;
	TSTreeCursor _cursor;
	std::string _doc;
};

static void loop_nodes(TSNode root, const std::string& src, std::pair<uint32_t, uint32_t> bounds, uint32_t depth, std::function<void(TSNode, uint32_t, std::string_view) > clb) {

	std::string type_name = ts_node_type(root);
	auto symbol = ts_node_symbol(root);
	auto start = ts_node_start_byte(root);
	auto end = ts_node_end_byte(root);

	if (type_name != "source_file") {
		if (start >= bounds.first || end <= bounds.second) {
			auto view = std::string_view(src.data() + start, src.data() + end);
			clb(root, depth, view);
		}
	}

	uint32_t child_count = ts_node_child_count(root);

	for (uint32_t i = 0; i < child_count; i++) {
		auto child = ts_node_child(root, i);
		loop_nodes(child, src, bounds, depth + 1, clb);
	}
}

QueryEditor::QueryEditor(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	
	{
		QFont font("Courier");
		font.setStyleHint(QFont::Monospace);
		font.setFixedPitch(true);
		ui.plainTextEdit->setFont(font);

		QFontMetrics metrics(font);
		ui.plainTextEdit->setTabStopDistance(metrics.averageCharWidth() * 2);
	}

	highlighter = new QueryHighlighter(this);
	highlighter->setDocument(ui.plainTextEdit->document());

	parser = ts_parser_new();
	lang = tree_sitter_WDBx();
	ts_parser_set_language(parser, lang);

	std::string str("");
	tree = ts_parser_parse_string(
		parser,
		nullptr,
		str.c_str(),
		str.length()
	);

	connect(ui.toolButtonReset, &QToolButton::pressed, [&]() {
		ui.plainTextEdit->clear();
		table->clearQuery();
	});

	connect(ui.toolButtonExecute, &QToolButton::pressed, [&]() {

		if (ui.plainTextEdit->document()->toPlainText().length() == 0) {
			table->clearQuery();
			return;
		}

		highlighter->rehighlight();

		try {
			std::string doc = parse();
			auto root_node = ts_tree_root_node(tree);

			ASTBuilder builder{ std::move(root_node), std::move(doc) };
			Query q = builder.build();

			table->query(std::move(q));
		}
		catch (std::exception& e) {
			QMessageBox::critical(this, "Query Error", QString::fromStdString(e.what()));
		}
	});
}

QueryEditor::~QueryEditor()
{
	ts_tree_delete(tree);
	ts_parser_delete(parser);
}

std::string QueryEditor::parse()
{
	std::string query_str = ui.plainTextEdit->document()->toPlainText().toStdString();

	ts_tree_delete(tree);

	tree = ts_parser_parse_string(
		parser,
		nullptr,
		query_str.c_str(),
		query_str.length()
	);

	return query_str;
}

void QueryHighlighter::highlightBlock(const QString& text)
{
	std::string doc = editor->parse();

	QTextBlock current = currentBlock();
	const auto start = current.position();
	const auto end = start + current.length();
	

	auto root_node = ts_tree_root_node(editor->tree);
	loop_nodes(root_node, doc, std::make_pair(start, end), 0, [&](TSNode node, uint32_t depth, std::string_view str) {

		std::string type_name = ts_node_type(node);
		auto symbol = ts_node_symbol(node);

		const auto node_start = ts_node_start_byte(node);
		const auto node_end = ts_node_end_byte(node);

		const auto fmt_start = node_start - start;
		const auto fmt_count = node_end - node_start;

		QTextCharFormat fmt;
		bool do_set = true;

		if (type_name == "ERROR") {
			fmt.setUnderlineStyle(QTextCharFormat::WaveUnderline);
			fmt.setUnderlineColor(Qt::red);
		}
		else if (type_name == "identifier") {
			fmt.setForeground(Qt::darkBlue);
		}
		else if (type_name == "string") {
			fmt.setForeground(Qt::darkRed);
		}
		else if (type_name == "number") {
			fmt.setForeground(Qt::darkGreen);
		}
		else if (type_name == "logic_op") {
			fmt.setForeground(Qt::darkMagenta);
		}
		else if (type_name == "(" || type_name == ")") {
			fmt.setForeground(Qt::darkGray);
		}
		else {
			do_set = false;
		}

		if (do_set) {
			setFormat(fmt_start, fmt_count, fmt);
		}
	});

}
