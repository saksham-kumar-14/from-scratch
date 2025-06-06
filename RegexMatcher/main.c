/*
 --REGEX FORMAT--
 ^ : matches beginning of input string
 $ : matches end of input string
 * : matches zero or more occurance of previous character
 . : matches any single character
 a : matches any literal character a
 * */

// Function for matching regex
int match(char *regex, char *text){
  if(regex[0] == '^')
    return matchFrom(regex + 1, text)

  while(*text++ != '\0'){
    if(matchFrom(regex, text))
      return 1;
  }
  return 0;
}

// function for literally matching both from start 
int matchFrom(char *regex, char* text){
  if(regex[0] == '\0')
    return 1;
  if(regex[1] == '*'){
    return matchStar(regex[0], regex + 2, text);
  }
  if(regex[0] == '$' && regex[1] == '\0')
    return *text == '\0';   // regex ends so text must end
  if(*text != '\0' && (regex[0] == '.' || regex[0] == *text))
    return matchFrom(++regex, ++text);
  return 0;
}

// function doing search for literal 'a' at beginning of the text
int matchStar(int a, char *regex, char *text){
  while(text != '\0' && (*text++ == a || a == '.'))
    if(matchFrom(regex, text))
      return 1;

  return 0;
}

int main(){

}
