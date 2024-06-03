#pragma once
#include "point.h"
#include "pointcloud.h"
#include <fstream>
#include <ios>
#include <ostream>
#include <string_view>
#include <utility>

namespace we {

template <typename VertexType, typename FaceType = we::Point3i>
class Mesh : public PointCloudBase<VertexType> {
  public:
    using Base = PointCloudBase<VertexType>;
    using face_type = FaceType;
    using span_face_type = std::span<face_type>;
    using const_span_face_type = std::span<const face_type>;
    using vector_face_type = std::vector<face_type>;

    Mesh() = default;

    Mesh(Base::span_type vertices, span_face_type faces)
        : Base{vertices}
        , faces_not_owned_{faces} {}

    Mesh(Base::vector_type &&vertices, vector_face_type &&faces) noexcept
        : Base{std::move(vertices)}
        , faces_owned_{std::move(faces)} {}

    void create(Base::vector_type &&vertices, vector_face_type &&faces) noexcept {
        Base::create(std::move(vertices));
        faces_owned_ = std::move(faces);
        faces_not_owned_ = {};
    }

    Mesh(Mesh &&) = default;
    Mesh(const Mesh &) noexcept = default;

    Mesh &operator=(const Mesh &) = default;
    Mesh &operator=(Mesh &&) noexcept = default;

    ~Mesh() = default;

    [[nodiscard]] size_t n_faces() const noexcept {
        return std::max(faces_owned_.size(), faces_not_owned_.size());
    }

    [[nodiscard]] span_face_type faces() noexcept {
        return faces_owned_.empty() ? faces_not_owned_ : faces_owned_;
    }

    [[nodiscard]] const_span_face_type faces() const noexcept {

        if(faces_not_owned_.empty()) {
            return faces_owned_;
        }

        return faces_not_owned_;
    }

    [[nodiscard]] bool empty() const noexcept {
        return Base::empty() and faces_owned_.empty() and faces_not_owned_.empty();
    }

    template <typename Writer> [[nodiscard]] bool write(const std::string_view path) const {
        std::ofstream f{path.data(), std::ios_base::binary};

        if(not f.is_open() or not f.good()) {
            return false;
        }

        write<Writer>(f);
        return true;
    }

    template <typename Writer> void write(std::ostream &stream) const {
        Writer writer{stream};
        write(writer);
    }

    template <typename Writer> void write(Writer &writer) const {
        Base::write(writer);
        writer.write_faces(faces());
    }

    template <typename Reader> [[nodiscard]] bool read(const std::string_view path) {
        std::ifstream f{path.data(), std::ios_base::binary};

        if(not f.is_open() or not f.good()) {
            return false;
        }

        return read<Reader>(f);
    }

    template <typename Reader> [[nodiscard]] bool read(std::istream &stream) {
        Reader reader{stream};
        return read(reader);
    }

    template <typename Reader> [[nodiscard]] bool read(Reader &reader) {
        faces_owned_.clear();
        faces_not_owned_ = {};

        if(not Base::read(reader)) {
            return false;
        }

        if(auto faces{reader.read_faces()}; faces) {
            faces_owned_ = std::move(*faces);
            return true;
        }

        return false;
    }

  private:
    vector_face_type faces_owned_;
    span_face_type faces_not_owned_;
};

using Mesh3f = Mesh<we::Point3f, we::Point3i>;

} // namespace we