#include "FAST/Data/BoundingBox.hpp"

namespace fast {

void BoundingBox::createCorners(Vector3f pos, Vector3f size) {
    // Create corners
    float corners[8][3] = {
        {pos.x(),pos.y(),pos.z()},
        {pos.x()+size.x(),pos.y(),pos.z()},
        {pos.x()+size.x(),pos.y()+size.y(),pos.z()},
        {pos.x()+size.x(),pos.y()+size.y(),pos.z()+size.z()},
        {pos.x(),pos.y()+size.y(),pos.z()+size.z()},
        {pos.x(),pos.y(),pos.z()+size.z()},
        {pos.x()+size.x(),pos.y(),pos.z()+size.z()},
        {pos.x(),pos.y()+size.y(),pos.z()}
    };

    mCorners = MatrixXf::Constant(8,3,0);
    for(int c = 0; c < 8; c++) {
        for(int i = 0; i < 3; i++) {
            mCorners(c,i) = corners[c][i];
        }
    }
}

BoundingBox::BoundingBox(Vector3f pos, Vector3f size) {
    mIsInitialized = true;
    createCorners(pos, size);
}

BoundingBox::BoundingBox(Vector3f size) {
    mIsInitialized = true;
    Vector3f pos;
    pos[0] = 0;
    pos[1] = 0;
    pos[2] = 0;
    createCorners(pos, size);
}

BoundingBox::BoundingBox(MatrixXf corners) {
    mIsInitialized = true;
    mCorners = corners; // copy
}

BoundingBox::BoundingBox() {
    mIsInitialized = false;
}

MatrixXf BoundingBox::getCorners() const {
    if(!mIsInitialized)
        throw Exception("Cannot getCorners because bounding box was not initialized.");
    return mCorners;
}


void BoundingBox::initialize(std::vector<Vector3f> coordinates) {
    if(coordinates.size() == 0) {
        mIsInitialized = true;
        createCorners(Vector3f::Zero(), Vector3f::Ones());
    }
    // Find min and max of all the coordinates
    Vector3f minimum(coordinates[0].x(), coordinates[0].y(), coordinates[0].z());
    Vector3f maximum(coordinates[0].x(), coordinates[0].y(), coordinates[0].z());
    for(uint i = 1; i < coordinates.size(); i++) {
        Vector3f coordinate = coordinates[i];
        for(uint j = 0; j < 3; j++) {
            if(coordinate[j] < minimum[j]) {
                minimum[j] = coordinate[j];
            }
            if(coordinate[j] > maximum[j]) {
                maximum[j] = coordinate[j];
            }
        }
    }

    // Make new bounding box
    Vector3f size(maximum.x()-minimum.x(), maximum.y()-minimum.y(), maximum.z()-minimum.z());
    mIsInitialized = true;
    createCorners(minimum, size);
}


void BoundingBox::initialize(std::vector<Vector2f> coordinates) {
    if(coordinates.size() == 0) {
        mIsInitialized = true;
        createCorners(Vector3f::Zero(), Vector3f::Ones());
    }
    // Find min and max of all the coordinates
    Vector3f minimum(coordinates[0].x(), coordinates[0].y(), 0);
    Vector3f maximum(coordinates[0].x(), coordinates[0].y(), 0);
    for(uint i = 1; i < coordinates.size(); i++) {
        Vector2f coordinate = coordinates[i];
        for(uint j = 0; j < 2; j++) {
            if(coordinate[j] < minimum[j]) {
                minimum[j] = coordinate[j];
            }
            if(coordinate[j] > maximum[j]) {
                maximum[j] = coordinate[j];
            }
        }
    }

    // Make new bounding box
    Vector3f size(maximum.x()-minimum.x(), maximum.y()-minimum.y(), 0);
    mIsInitialized = true;
    createCorners(minimum, size);
}

BoundingBox::BoundingBox(std::vector<Vector2f> coordinates) {
	initialize(coordinates);
}

BoundingBox::BoundingBox(std::vector<Vector3f> coordinates) {
	initialize(coordinates);
}

BoundingBox::BoundingBox(std::vector<VectorXf> coordinates) {
    // Find min and max of all the coordinates
	if(coordinates[0].size() == 2) {
		std::vector<Vector2f> coordinates2;
		for(VectorXf v : coordinates) {
			coordinates2.push_back(Vector2f(v.x(), v.y()));
		}
		initialize(coordinates2);
	} else {
		std::vector<Vector3f> coordinates2;
		for(VectorXf v : coordinates) {
			coordinates2.push_back(Vector3f(v.x(), v.y(), v.z()));
		}
		initialize(coordinates2);
	}
}

BoundingBox BoundingBox::getTransformedBoundingBox(
        AffineTransformation::pointer transform) const {
    if(!mIsInitialized)
        throw Exception("Cannot getTransformedBoundingBox because bounding box was not initialized.");
    MatrixXf newCorners = MatrixXf::Constant(8,3,0);
    for(uint i = 0; i < 8; i++) {
        Vector3f vertex = mCorners.row(i);
        Vector3f transformedVertex = (transform->matrix()*vertex.homogeneous()).head(3);
        newCorners.row(i) = transformedVertex;
    }
    return BoundingBox(newCorners);
}

std::ostream &operator<<(std::ostream &os, BoundingBox &object) {
    os << std::endl << "Bounding box" << std::endl;

    MatrixXf corners = object.getCorners();

    for(uint i = 0; i < 8; i++) {
        os << "Corner " << i << ": " << corners(i,0) << ", " << corners(i, 1) << ", " << corners(i, 2) << std::endl;
    }

    return os;

}

bool BoundingBox::isInitialized() const {
    return mIsInitialized;
}

} // end namespace fast

