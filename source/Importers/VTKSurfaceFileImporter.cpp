#include "VTKSurfaceFileImporter.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>

namespace fast {

void VTKSurfaceFileImporter::setFilename(std::string filename) {
    mFilename = filename;
    mIsModified = true;
}

Surface::pointer VTKSurfaceFileImporter::getOutput() {
    mOutput->setSource(mPtr);
    return mOutput;
}

VTKSurfaceFileImporter::VTKSurfaceFileImporter() {
    mOutput = Surface::New();
    mFilename = "";
    mIsModified = true;
}

bool gotoLineWithString(std::ifstream &file, std::string searchFor) {
    bool found = false;
    std::string line;
    while(getline(file, line)) {
        if(line.find(searchFor) != std::string::npos) {
            found = true;
            break;
        }
    }

    return found;
}

void VTKSurfaceFileImporter::execute() {
    if(mFilename == "")
        throw Exception("No filename given to the VTKSurfaceFileImporter");

    // Try to open file and check that it exists
    std::ifstream file(mFilename.c_str());
    std::string line;
    if(!file.is_open()) {
        throw FileNotFoundException(mFilename);
    }

    // Check file header?

    // Read vertices
    std::vector<Float3> vertices;
    if(!gotoLineWithString(file, "POINTS")) {
        throw Exception("Found no vertices in the VTK surface file");
    }
    while(getline(file, line)) {
        boost::trim(line);
        if(line.size() == 0)
            break;

        if(!(isdigit(line[0]) || line[0] == '-')) {
            // Has reached end
            break;
        }

        std::vector<std::string> tokens;
        boost::split(tokens, line, boost::is_any_of(" "));

        for(int i = 0; i < tokens.size(); i += 3) {
            Float3 v;
            v[0] = boost::lexical_cast<float>(tokens[i]);
            v[1] = boost::lexical_cast<float>(tokens[i+1]);
            v[2] = boost::lexical_cast<float>(tokens[i+2]);
            vertices.push_back(v);
        }
    }
    file.seekg(0); // set stream to start

    // Read triangles (other types of polygons not supported yet)
    std::vector<Uint3> triangles;
    if(!gotoLineWithString(file, "POLYGONS")) {
        throw Exception("Found no triangles in the VTK surface file");
    }
    while(getline(file, line)) {
        boost::trim(line);
        if(line.size() == 0)
            break;

        if(!isdigit(line[0])) {
            // Has reached end
            break;
        }


        std::vector<std::string> tokens;
        boost::split(tokens, line, boost::is_any_of(" "));

        if(boost::lexical_cast<int>(tokens[0]) != 3) {
            throw Exception("The VTKSurfaceFileImporter currently only supports reading files with triangles. Encountered a non-triangle. Aborting.");
        }
        if(tokens.size() != 4) {
            throw Exception("Error while reading triangles in VTKSurfaceFileImporter. Check format.");
        }

        Uint3 triangle;
        triangle[0] = boost::lexical_cast<uint>(tokens[1]);
        triangle[1] = boost::lexical_cast<uint>(tokens[2]);
        triangle[2] = boost::lexical_cast<uint>(tokens[3]);

        triangles.push_back(triangle);

    }
    file.seekg(0); // set stream to start

    // Read normals (if any)
    std::vector<Float3> normals;
    if(!gotoLineWithString(file, "NORMALS")) {
        throw Exception("Found no normals in the VTK surface file");
    }
    while(getline(file, line)) {
        boost::trim(line);
        if(line.size() == 0)
            break;

        if(!(isdigit(line[0]) || line[0] == '-')) {
            // Has reached end
            break;
        }

        std::vector<std::string> tokens;
        boost::split(tokens, line, boost::is_any_of(" "));

        for(int i = 0; i < tokens.size(); i += 3) {
            Float3 v;
            v[0] = boost::lexical_cast<float>(tokens[i]);
            v[1] = boost::lexical_cast<float>(tokens[i+1]);
            v[2] = boost::lexical_cast<float>(tokens[i+2]);
            normals.push_back(v);
        }
    }

    if(normals.size() != vertices.size()) {
        std::string message = "Read different amount of vertices (" + boost::lexical_cast<std::string>(vertices.size()) + ") and normals (" + boost::lexical_cast<std::string>(normals.size()) + ").";
        throw Exception(message);
    }

    // Add data to mOutput
    mOutput->create(vertices, normals, triangles);

    // Update timestamp on output
    mOutput->updateModifiedTimestamp();
}

} // end namespace fast

