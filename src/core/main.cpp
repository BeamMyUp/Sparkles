/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob

    Nori is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Nori is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <nori/core/parser.h>
#include <nori/core/gui.h>
#include <nori/core/block.h>
#include <nori/core/scene.h>
#include <nori/core/bitmap.h>
#include <glviewer/viewer.h>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <filesystem/resolver.h>
#include <thread>

using namespace nori;

static void renderBlock(const Scene *scene, Sampler *sampler, ImageBlock &block) {
  
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Syntax: " << argv[0] << " <scene.xml>" << endl;
        return -1;
    }

    filesystem::path path(argv[1]);

    try {
        if (path.extension() == "xml") {
            /* Add the parent directory of the scene file to the
               file resolver. That way, the XML file can reference
               resources (OBJ files, textures) using relative paths */
            getFileResolver()->prepend(path.parent_path());

            std::unique_ptr<NoriObject> root(loadFromXML(argv[1]));

            /* When the XML root object is a scene, start rendering it .. */
			if (root->getClassType() == NoriObject::EClassType::EScene) {
				viewer::Viewer& viewer = viewer::Viewer::getInstance();
				viewer.initialize(static_cast<Scene *>(root.get()));
				viewer.render(argv[1]);
			}
                
        } else if (path.extension() == "exr") {
            /* Alternatively, provide a basic OpenEXR image viewer */
            nori::Bitmap bitmap(argv[1]);
            nori::ImageBlock block(Vector2i((int) bitmap.cols(), (int) bitmap.rows()), nullptr);
            block.fromBitmap(bitmap);
            nanogui::init();
            NoriScreen *screen = new NoriScreen(block);
            nanogui::mainloop();
            delete screen;
            nanogui::shutdown();
        } else {
            cerr << "Fatal error: unknown file \"" << argv[1]
                 << "\", expected an extension of type .xml or .exr" << endl;
        }
    } catch (const std::exception &e) {
        cerr << "Fatal error: " << e.what() << endl;
        return -1;
    }
    return 0;
}
