/*
For more information, please see: http://software.sci.utah.edu

The MIT License

Copyright (c) 2015 Scientific Computing and Imaging Institute,
University of Utah.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include <Interface/Modules/Base/CustomWidgets/CodeEditorWidgets.h>

using namespace SCIRun::Gui;

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
  lineNumberArea_ = new LineNumberArea(this);

  connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
  connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
  connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

  updateLineNumberAreaWidth(0);
  highlightCurrentLine();

  highlighter_ = new Highlighter(document());

  connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(matchParentheses()));
}

int CodeEditor::lineNumberAreaWidth()
{
  int digits = 1;
  int max = qMax(1, blockCount());
  while (max >= 10) {
    max /= 10;
    ++digits;
  }

  int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

  return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
  setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
  if (dy)
    lineNumberArea_->scroll(0, dy);
  else
    lineNumberArea_->update(0, rect.y(), lineNumberArea_->width(), rect.height());

  if (rect.contains(viewport()->rect()))
    updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
  QPlainTextEdit::resizeEvent(e);

  QRect cr = contentsRect();
  lineNumberArea_->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> extras;// = extraSelections();

  if (!isReadOnly())
  {
    QTextEdit::ExtraSelection selection;

    QColor lineColor = QColor(Qt::darkGray);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extras.append(selection);
  }

  setExtraSelections(extras);
}

void CodeEditor::insertSpecialCodeBlock()
{
  insertPlainText("/*matlab\nmatlab*/");
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
  QPainter painter(lineNumberArea_);
  painter.fillRect(event->rect(), Qt::lightGray);
  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom = top + (int)blockBoundingRect(block).height();
  while (block.isValid() && top <= event->rect().bottom())
  {
    if (block.isVisible() && bottom >= event->rect().top())
    {
      QString number = QString::number(blockNumber + 1);
      painter.setPen(Qt::black);
      painter.drawText(0, top, lineNumberArea_->width(), fontMetrics().height(),
        Qt::AlignRight, number);
    }

    block = block.next();
    top = bottom;
    bottom = top + (int)blockBoundingRect(block).height();
    ++blockNumber;
  }
}

void CodeEditor::matchParentheses()
{
  QList<QTextEdit::ExtraSelection> selections;
  setExtraSelections(selections);
  auto data = static_cast<TextBlockData*>(textCursor().block().userData());

  if (data)
  {
    auto infos = data->parentheses();
    for (int i = 0; i < infos.size(); ++i)
    {
      auto info = infos[i];
      int curPos = textCursor().position() - textCursor().block().position();
      if (info.position == curPos - 1 && info.character == '(')
      {
        if (!matchLeftParenthesis(textCursor().block(), i + 1, 0))
        {
          createParenthesisSelection(info.position, Qt::red);
        }
      }
      else if (info.position == curPos - 1 && info.character == ')')
      {
        if (!matchRightParenthesis(textCursor().block(), i - 1, 0))
        {
          createParenthesisSelection(info.position, Qt::red);
        }
      }
    }
  }
}

void CodeEditor::createParenthesisSelection(int pos, const QColor& color)
{
  auto selections = extraSelections();
  QTextEdit::ExtraSelection selection;
  selection.format.setBackground(color);
  QTextCursor cursor = textCursor();
  cursor.setPosition(pos);
  cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
  selection.cursor = cursor;
  selections.append(selection);
  setExtraSelections(selections);
}

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
  HighlightingRule rule;

  keywordFormat.setForeground(Qt::green);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList keywordPatterns;
  keywordPatterns << "\\band\\b" << "\\bclass\\b" << "\\bassert\\b"
    << "\\bbreak\\b" << "\\bas\\b" << "\\bcontinue\\b"
    << "\\bdef\\b" << "\\bdel\\b" << "\\belif\\b"
    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
    << "\\belse\\b" << "\\bexcept\\b" << "\\bexec\\b"
    << "\\bfinally\\b" << "\\bfrom\\b" << "\\bimport\\b"
    << "\\bglobal\\b" << "\\bif\\b" << "\\bis\\b"
    << "\\blambda\\b" << "\\bnot\\b" << "\\bor\\b"
    << "\\bpass\\b" << "\\braise\\b" << "\\breturn\\b"
    << "\\btry\\b" << "\\bwhile\\b" << "\\bfor\\b" << "\\bin\\b"
    << "\\bwith\\b" << "\\byield\\b"
    << "\\bTrue\\b" << "\\bFalse\\b" << "\\bNone\\b";
  for (const auto& pattern : keywordPatterns)
  {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }
  classFormat.setFontWeight(QFont::Bold);
  classFormat.setForeground(Qt::darkMagenta);
  rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
  rule.format = classFormat;
  highlightingRules.append(rule);

  quotationFormat.setForeground(Qt::darkGreen);
  rule.pattern = QRegExp("\".*\"");
  rule.format = quotationFormat;
  highlightingRules.append(rule);

  functionFormat.setFontItalic(true);
  functionFormat.setForeground(Qt::cyan);
  rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
  rule.format = functionFormat;
  highlightingRules.append(rule);

  singleLineCommentFormat.setForeground(Qt::yellow);
  rule.pattern = QRegExp("#[^\n]*");
  rule.format = singleLineCommentFormat;
  highlightingRules.append(rule);

  multiLineCommentFormat.setForeground(QColor(255,105,180));

  commentStartExpression = QRegExp("/\\*matlab");
  commentEndExpression = QRegExp("matlab\\*/");
}

void Highlighter::highlightBlock(const QString &text)
{
  highlightBlockParens(text);
  for (const auto& rule : highlightingRules)
  {
    QRegExp expression(rule.pattern);
    int index = expression.indexIn(text);
    while (index >= 0)
    {
      int length = expression.matchedLength();
      setFormat(index, length, rule.format);
      index = expression.indexIn(text, index + length);
    }
  }

  setCurrentBlockState(0);
  int startIndex = 0;
  if (previousBlockState() != 1)
    startIndex = commentStartExpression.indexIn(text);

  while (startIndex >= 0)
  {
    int endIndex = commentEndExpression.indexIn(text, startIndex);
    int commentLength;
    if (endIndex == -1)
    {
      setCurrentBlockState(1);
      commentLength = text.length() - startIndex;
    }
    else
    {
      commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
    }
    setFormat(startIndex, commentLength, multiLineCommentFormat);
    startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
  }
}

void Highlighter::highlightBlockParens(const QString &text)
{
  auto data = new TextBlockData;
  int leftPos = text.indexOf('(');
  while (leftPos != -1)
  {
    data->insert({'(', leftPos});
    leftPos = text.indexOf('(', leftPos + 1);
  }
  int rightPos = text.indexOf(')');
  while (rightPos != -1)
  {
    data->insert({ ')', rightPos });
    rightPos = text.indexOf(')', rightPos + 1);
  }
  setCurrentBlockUserData(data);
}

bool CodeEditor::matchLeftParenthesis(QTextBlock currentBlock, int i, int numLeftParentheses)
{
  auto data = static_cast<TextBlockData *>(currentBlock.userData());
  auto infos = data->parentheses();

  int docPos = currentBlock.position();
  for (; i < infos.size(); ++i)
  {
    auto info = infos[i];

    if (info.character == '(')
    {
      ++numLeftParentheses;
      continue;
    }

    if (info.character == ')' && numLeftParentheses == 0)
    {
      createParenthesisSelection(docPos + info.position, Qt::green);
      return true;
    }
    else
      --numLeftParentheses;
  }

  currentBlock = currentBlock.next();
  if (currentBlock.isValid())
    return matchLeftParenthesis(currentBlock, 0, numLeftParentheses);

  return false;
}

bool CodeEditor::matchRightParenthesis(QTextBlock currentBlock, int i, int numRightParentheses)
{
  auto data = static_cast<TextBlockData *>(currentBlock.userData());
  auto parentheses = data->parentheses();

  int docPos = currentBlock.position();
  for (; i > -1 && parentheses.size() > 0; --i)
  {
    auto info = parentheses.at(i);
    if (info.character == ')')
    {
      ++numRightParentheses;
      continue;
    }
    if (info.character == '(' && numRightParentheses == 0)
    {
      createParenthesisSelection(docPos + info.position, Qt::green);
      return true;
    }
    else
      --numRightParentheses;
  }

  currentBlock = currentBlock.previous();
  if (currentBlock.isValid())
    return matchRightParenthesis(currentBlock, 0, numRightParentheses);

  return false;
}

TextBlockData::TextBlockData()
{
}

std::vector<ParenthesisInfo> TextBlockData::parentheses() const
{
  return m_parentheses;
}


void TextBlockData::insert(ParenthesisInfo&& info)
{
  int i = 0;
  while (i < m_parentheses.size() && info.position > m_parentheses[i].position)
    ++i;

  m_parentheses.insert(m_parentheses.begin() + i, info);
}
