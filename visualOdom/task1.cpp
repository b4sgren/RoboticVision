#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <glob.h>
#include <iostream>

int main()
{
  std::string filepath{"../PracticeImgs/PracticeSequence/"};
  std::string num{"000"};
  std::string type{".png"};

  glob_t result;
  std::string glob_path = filepath + "*" + type;
  glob(glob_path.c_str(), GLOB_TILDE, NULL, &result);
  std::vector<std::string> filenames;
  for(size_t i{0}; i < result.gl_pathc; i++)
    filenames.push_back(std::string(result.gl_pathv[i]));

  std::cout << filenames.size();

  return 0;
}
