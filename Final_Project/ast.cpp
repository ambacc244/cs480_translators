#include <sstream>
#include <iostream>
#include <set>

#include "ast.hpp"
#include "parser.hpp"

/*
 * Simple template function to convert a value of any type to a string
 * representation.  The type must have an insertion operator (i.e. operator<<).
 */
 int index = 1;
 bool addIndex = false;
 bool subIndex = false;
 bool mulIndex = false;
 bool divIndex = false;
 std::set<std::string> symbols;

template <class T>
std::string toString(const T& val) {
  std::ostringstream out;
  out << val;
  return out.str();
}

bool isNumber(std::string nodeName){
  std::string tempName = nodeName;
  if((tempName.find_first_not_of( "0123456789." ) == std::string::npos))
    return true;
  else
    return false;
}

std::string numberConvert(std::string nodeName){
  float num = std::stof(nodeName);
  int i = 10;

  while (num > 10) {
    num = num/10.0;
    i--;
  }
  nodeName = toString(num) + "e+";

  if(i<10)
    nodeName += "0" + toString(10-i);
  else
    nodeName +=toString(10-i);
  return nodeName;
}

/*
 * Function to generate GraphView spec for the subtree rooted at any node in
 * an AST.
 *
 * @param node An AST node.
 *
 * @return Returns a string containing a complete GraphView specification to
 *   visualize the AST subtree rooted at node.
 */
std::string generateGVSpec(ASTNode* node) {
  std::string gvSpec = "";
  std::string nodeName = "";
  node->generateGVSpec(nodeName, gvSpec);
  gvSpec += "  ret void\n";
  gvSpec += "}\n";
  return gvSpec;
}

/****************************************************************************
 **
 ** Below is the implementation of generateGVSpec() for each class defined in
 ** ast.hpp.  Please see the documentation in ast.hpp for a desription of
 ** this function.
 **
 ****************************************************************************/

void ASTIdentifier::generateGVSpec(std::string& nodeName, std::string& gvSpec) const {
  symbols.insert(*this->name);
  nodeName = "%" + *this->name;
}


void ASTFloat::generateGVSpec(std::string& nodeName, std::string& gvSpec) const {
  nodeName = toString(this->value);
}


void ASTInteger::generateGVSpec(std::string& nodeName, std::string& gvSpec) const {
  nodeName = toString(this->value);
}


void ASTBoolean::generateGVSpec(std::string& nodeName, std::string& gvSpec) const {
  if(this->value == true)
    nodeName = toString(1);
  else
    nodeName = toString(0);

}


void ASTBinaryOperatorExpression::generateGVSpec(std::string& nodeName, std::string& gvSpec) const {

  std::string plhsNodeName = "";
  std::string prhsNodeName = "";
  std::string lhsNodeName = "";
  std::string rhsNodeName = "";

  int lhsIndex;
  int rhsIndex;

  std::string opStr;

  this->lhs->generateGVSpec(nodeName, gvSpec);
  plhsNodeName = nodeName;
  lhsNodeName = plhsNodeName;

  if(isNumber(lhsNodeName)){
    lhsIndex = index;
  }
  else if(!(lhsNodeName.find("%addtmp") && lhsNodeName.find("%subtmp") && lhsNodeName.find("%multmp") && lhsNodeName.find("%divtmp"))){
    lhsIndex = index;
  }else{
    lhsIndex = index++;
    lhsNodeName += toString(lhsIndex);
  }

  this->rhs->generateGVSpec(nodeName, gvSpec);
  prhsNodeName = nodeName;
  rhsNodeName = prhsNodeName;

  if(isNumber(prhsNodeName)){
    rhsIndex = index;
  }
  else{
    rhsIndex = index++;
    rhsNodeName += toString(rhsIndex);
  }

  switch (this->op) {
    case PLUS:
      opStr = "%add";
      if(addIndex ==false){
        nodeName = opStr + "tmp";
        addIndex = true;
      }
      else{
        nodeName = opStr + "tmp" + toString(index++);
      }
      break;
    case MINUS:
      opStr = "%sub";
      if(subIndex ==false){
        nodeName = opStr + "tmp";
        subIndex = true;
      }
      else{
        nodeName = opStr + "tmp" + toString(index++);
      }
      break;
    case TIMES:
      opStr = "%mul";
      if(mulIndex ==false){
        nodeName = opStr + "tmp";
        mulIndex = true;
      }
      else{
        nodeName = opStr + "tmp" + toString(index++);
      }
      break;
    case DIVIDEDBY:
      opStr = "%div";
      if(divIndex ==false){
        nodeName = opStr + "tmp";
        divIndex = true;
      }
      else{
        nodeName = opStr + "tmp" + toString(index++);
      }
      break;
    case EQ:
      opStr = "%eq";
      nodeName = opStr + "tmp" + toString(index++);
      break;
    case NEQ:
      opStr = "%neq";
      nodeName = opStr + "tmp" + toString(index++);
      break;
    case GT:
      opStr = "%gt";
      nodeName = opStr + "tmp" + toString(index++);
      break;
    case GTE:
      opStr = "%gte";
      nodeName = opStr + "tmp" + toString(index++);
      break;
    case LT:
      opStr = "%lt";
      nodeName = opStr + "tmp" + toString(index++);
      break;
    case LTE:
      opStr = "%lte";
      nodeName = opStr + "tmp" + toString(index++);
      break;
  }

  if(isNumber(lhsNodeName) && isNumber(rhsNodeName)){
    switch (this->op) {
      case PLUS:
        nodeName = toString(std::stof(lhsNodeName) + std::stof(rhsNodeName));
        break;
      case MINUS:
        nodeName = toString(std::stof(lhsNodeName) - std::stof(rhsNodeName));
        break;
      case TIMES:
        nodeName = toString(std::stof(lhsNodeName) * std::stof(rhsNodeName));
        break;
      case DIVIDEDBY:
        nodeName = toString(std::stof(lhsNodeName) / std::stof(rhsNodeName));
        break;
    }
  }
  else{
    if(!isNumber(lhsNodeName) && (lhsNodeName.find("%addtmp") && lhsNodeName.find("%subtmp") && lhsNodeName.find("%multmp") && lhsNodeName.find("%divtmp")
                              && lhsNodeName.find("%eqtmp") && lhsNodeName.find("%neqtmp") && lhsNodeName.find("%gttmp")
                              && lhsNodeName.find("%gtetmp") && lhsNodeName.find("%lttmp") && lhsNodeName.find("%ltetmp")))
      gvSpec += "  " + lhsNodeName + " = load float, float* " + plhsNodeName + "\n";

    if(!isNumber(rhsNodeName))
      gvSpec += "  " + rhsNodeName + " = load float, float* " + prhsNodeName + "\n";

    if(!(opStr.compare("%eq") && opStr.compare("%neq") && opStr.compare("%gt")
      && opStr.compare("%gte") && opStr.compare("%lt") && opStr.compare("%lte")))
      gvSpec += "  " + nodeName + " = fcmp ugh float " + lhsNodeName + ", " + rhsNodeName + "\n";
    else
      gvSpec += "  " + nodeName + " = f" + opStr.substr(1) + " float " + lhsNodeName + ", " + rhsNodeName + "\n";

  }
}


void ASTAssignmentStatement::generateGVSpec(std::string& nodeName, std::string& gvSpec) const {

  std::string pNodeName = nodeName;
  std::string NodeName;

  this->rhs->generateGVSpec(nodeName, gvSpec);
  NodeName = nodeName;



  if(!isNumber(NodeName) && pNodeName.compare(NodeName)
  && (NodeName.find("%addtmp") && NodeName.find("%subtmp") && NodeName.find("%multmp") && NodeName.find("%divtmp")
  && NodeName.find("%eqtmp") && NodeName.find("%neqtmp") && NodeName.find("%gttmp")
  && NodeName.find("%gtetmp") && NodeName.find("%lttmp") && NodeName.find("%ltetmp"))){
    pNodeName = NodeName;
    NodeName += toString(index++);
    gvSpec += "  " + NodeName + " = load float, float* " + pNodeName + "\n";
  }

  gvSpec += "  store float ";
  gvSpec +=  NodeName;
  gvSpec += ", float* ";
  this->lhs->generateGVSpec(nodeName, gvSpec);
  gvSpec += nodeName + "\n";
}


void ASTBlock::generateGVSpec(std::string& nodeName, std::string& gvSpec) const {

  for (int i = 0; i < this->statements.size(); i++) {
    this->statements[i]->generateGVSpec(nodeName, gvSpec);
  }

}


void ASTIfStatement::generateGVSpec(std::string& nodeName, std::string& gvSpec) const {

  std::string pcondNodeName1 = "";
  std::string condNodeName1 = "";
  std::string condNodeName2 = "";

  std::string ifNodeName = "";
  std::string elseNodeName = "";
  std::string continueNodeName = "";

  int condIndex1;
  int condIndex2;

  int ifIndex;
  int elseIndex;
  int continueIndex;

  this->condition->generateGVSpec(nodeName, gvSpec);
  pcondNodeName1 = nodeName;



  if(!(pcondNodeName1.find("%eqtmp") && pcondNodeName1.find("%neqtmp") && pcondNodeName1.find("%gttmp")
    && pcondNodeName1.find("%gtetmp") && pcondNodeName1.find("%lttmp") && pcondNodeName1.find("%ltetmp"))){
    condNodeName1 = "%booltmp";
  }
  else{
    condNodeName1 = pcondNodeName1;
  }

  condIndex1 = index++;
  condNodeName1 += toString(condIndex1);

  condNodeName2 = "%ifcond";
  condIndex2 = index++;
  condNodeName2 += toString(condIndex2);

  if(this->elseBlock == NULL){

      ifNodeName = "%ifBlock";
      ifIndex = index++;
      ifNodeName += toString(ifIndex);

      continueNodeName = "%ifContinueBlock";
      continueIndex = index++;
      continueNodeName += toString(continueIndex);

      if(!(pcondNodeName1.find("%eqtmp") && pcondNodeName1.find("%neqtmp") && pcondNodeName1.find("%gttmp")
        && pcondNodeName1.find("%gtetmp") && pcondNodeName1.find("%lttmp") && pcondNodeName1.find("%ltetmp")))
        gvSpec += "  " + condNodeName1 + " uitofp il " + pcondNodeName1 + " to float\n";
      else
        gvSpec += "  " + condNodeName1 + " = load float, float* " + pcondNodeName1 + "\n";

      gvSpec += "  " + condNodeName2 + " = fcmp one float " + condNodeName1 + ", 0.000000e+00\n";
      gvSpec += "  br i1 " + condNodeName2 + ", label " + ifNodeName + ", label " + continueNodeName + "\n";

      gvSpec += "\n" + ifNodeName.substr(1) + ":\n";
      this->ifBlock->generateGVSpec(nodeName, gvSpec);
      gvSpec += "  br label " + continueNodeName + "\n";

      gvSpec += "\n" + continueNodeName.substr(1) + ":\n";

    }
    else{
      ifNodeName = "%ifBlock";
      ifIndex = index++;
      ifNodeName += toString(ifIndex);

      elseNodeName = "%elseBlock";
      elseIndex = index++;
      elseNodeName += toString(elseIndex);

      continueNodeName = "%ifContinueBlock";
      continueIndex = index++;
      continueNodeName += toString(continueIndex);

      gvSpec += "  " + condNodeName1 + " = load float, float* " + pcondNodeName1 + "\n";
      gvSpec += "  " + condNodeName2 + " = fcmp one float " + condNodeName1 + ", 0.000000e+00\n";
      gvSpec += "  br i1 " + condNodeName2 + ", label " + ifNodeName + ", label " + elseNodeName + "\n";

      gvSpec += "\n" + ifNodeName.substr(1) + ":\n";
      this->ifBlock->generateGVSpec(nodeName, gvSpec);
      gvSpec += "  br label " + continueNodeName + "\n";

      gvSpec += "\n" + elseNodeName.substr(1) + ":\n";
      this->elseBlock->generateGVSpec(nodeName, gvSpec);
      gvSpec += "  br label " + continueNodeName + "\n";

      gvSpec += "\n" + continueNodeName.substr(1) + ":\n";
    }
}


void ASTWhileStatement::generateGVSpec(std::string& nodeName, std::string& gvSpec) const {

  std::string pcondNodeName1 = "";
  std::string condNodeName1 = "";
  std::string condNodeName2 = "";

  int pcondIndex1;
  int condIndex1;
  int condIndex2;

  std::string whileNodeName = "";
  std::string continueNodeName = "";

  int whileIndex;
  int continueIndex;

  condNodeName2 = "%whileCondBlock";
  condIndex2 = index++;
  condNodeName2 += toString(condIndex2);

  whileNodeName = "%whileBlock";
  whileIndex = index++;
  whileNodeName += toString(whileIndex);

  continueNodeName = "%whileContinueBlock";
  continueIndex = index++;
  continueNodeName += toString(continueIndex);

  gvSpec += "  br label " + condNodeName2 + "\n";

  gvSpec += "\n" + condNodeName2.substr(1) + ":\n";

  this->condition->generateGVSpec(nodeName, gvSpec);
  pcondNodeName1 = nodeName;

  if(isNumber(nodeName)){
    if(std::stoi(nodeName) == 1)
      condNodeName1 = "true";
    else if(std::stoi(nodeName) == 0)
      condNodeName1 = "false";
  }
  else{
    condNodeName1 = pcondNodeName1;
    condIndex1 = index++;
    condNodeName1 += toString(condIndex1);
  }

  gvSpec += "  br i1 " + condNodeName1 + ", label " + whileNodeName + ", label " + continueNodeName + "\n";

  gvSpec += "\n" + whileNodeName.substr(1) + ":\n";
  this->whileBlock->generateGVSpec(nodeName, gvSpec);
  gvSpec += "  br label " + condNodeName2 + "\n";

  gvSpec += "\n" + continueNodeName.substr(1) + ":\n";

}


void ASTBreakStatement::generateGVSpec(std::string& nodeName, std::string& gvSpec) const {

}
