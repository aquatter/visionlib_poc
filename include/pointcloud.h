#pragma once
#include "point.h"
#include "we_assert.h"
#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace we {

enum class Prop { NORMALS, INTENSITY, CONFIDENCE, RGB, LAST_PROP };

namespace detail {

#define PROP_NAME_INFO(F)                                                                          \
    F(we::Prop::NORMALS, we::Point3f, "normals")                                                   \
    F(we::Prop::INTENSITY, uint16_t, "intensity")                                                  \
    F(we::Prop::CONFIDENCE, uint16_t, "confidence")                                                \
    F(we::Prop::RGB, we::Point3ub, "rgb")

template <we::Prop name> struct prop_traits {};

#define DECLARE_PROP_TRAITS_(param_name, T, str)                                                   \
    template <> struct prop_traits<param_name> {                                                   \
        using type = T;                                                                            \
        static constexpr std::string_view tag{str};                                                \
    };

PROP_NAME_INFO(DECLARE_PROP_TRAITS_)

template <we::Prop name> using prop_traits_t = typename prop_traits<name>::type;

template <we::Prop name> using prop_span_t = std::span<typename prop_traits<name>::type>;

template <we::Prop name>
using prop_const_span_t = std::span<const typename prop_traits<name>::type>;

template <we::Prop name> using prop_vector_t = std::vector<typename prop_traits<name>::type>;

template <we::Prop name>
using prop_opt_vector_t = std::optional<std::vector<typename prop_traits<name>::type>>;

template <we::Prop name> constexpr inline std::string_view prop_traits_v = prop_traits<name>::tag;

} // namespace detail

struct BaseProperty {
    BaseProperty(const std::string_view name, const std::string_view type_name)
        : name_{name}
        , type_name_{type_name} {}

    BaseProperty(const BaseProperty &rhs) = default;
    BaseProperty(BaseProperty &&rhs) noexcept = default;

    BaseProperty &operator=(const BaseProperty &) = default;
    BaseProperty &operator=(BaseProperty &&) noexcept = default;

    virtual ~BaseProperty() = default;
    virtual void resize(size_t n) = 0;
    virtual std::unique_ptr<BaseProperty> clone() const = 0;

    std::string name_;
    std::string type_name_;
};

template <typename T> class Property : public BaseProperty {
  public:
    using value_type = T;
    using vector_type = std::vector<T>;
    using span_type = std::span<T>;
    using const_span_type = std::span<const T>;
    using reference = typename vector_type::reference;
    using const_reference = typename vector_type::const_reference;

    Property(const std::string_view name, const std::string_view type_name)
        : BaseProperty(name, type_name) {}

    Property(const std::string_view name, const std::string_view type_name, vector_type &&data)
        : BaseProperty(name, type_name)
        , data_{std::move(data)} {}

    Property(const Property &rhs) = default;
    Property(Property &&rhs) noexcept = default;

    Property &operator=(const Property &) = default;
    Property &operator=(Property &&) noexcept = default;

    void resize(size_t n) override { data_.resize(n); }

    [[nodiscard]] span_type data() { return data_; }

    [[nodiscard]] const const_span_type data() const { return data_; }

    [[nodiscard]] std::unique_ptr<BaseProperty> clone() const override {
        return std::make_unique<Property<T>>(*this);
    }

  private:
    vector_type data_;
};

template <typename T> class PropertyHandle {
  public:
    using value_type = T;
    using vector_type = std::vector<T>;
    using span_type = std::span<T>;
    using const_span_type = std::span<const T>;
    using reference = typename vector_type::reference;
    using const_reference = typename vector_type::const_reference;

    explicit PropertyHandle(int idx = -1)
        : idx_{idx} {}

    [[nodiscard]] int indx() const noexcept { return idx_; }
    [[nodiscard]] bool is_valid() const noexcept { return idx_ != -1; }
    void reset() { idx_ = -1; }

  private:
    int idx_;
};

class PropertyContainer {
  public:
    PropertyContainer() = default;
    PropertyContainer(const PropertyContainer &rhs) { operator=(rhs); }
    PropertyContainer(PropertyContainer &&) noexcept = default;
    PropertyContainer &operator=(PropertyContainer &&) noexcept = default;

    PropertyContainer &operator=(const PropertyContainer &rhs) {
        if(&rhs == this) {
            return *this;
        }

        properties_.resize(rhs.properties_.size());

        std::transform(rhs.properties_.cbegin(), rhs.properties_.cend(), properties_.begin(),
                       [](auto &&val) { return val->clone(); });

        return *this;
    }

    template <typename T> [[nodiscard]] PropertyHandle<T> add(const std::string_view name) {
        return add<T>(name, {});
    }

    template <typename T>
    [[nodiscard]] PropertyHandle<T> add(const std::string_view name,
                                        PropertyHandle<T>::vector_type &&data) {
        const auto it{std::find_if(properties_.begin(), properties_.end(),
                                   [](auto &&val) { return not val; })};
        int ind{0};

        if(it != properties_.end()) {
            ind = static_cast<int>(std::distance(properties_.begin(), properties_.end()));
            properties_[ind] =
                std::make_unique<Property<T>>(name, typeid(T).raw_name(), std::move(data));
        } else {
            ind = static_cast<int>(properties_.size());
            properties_.emplace_back(
                std::make_unique<Property<T>>(name, typeid(T).raw_name(), std::move(data)));
        }

        return PropertyHandle<T>{ind};
    }

    template <typename T> [[nodiscard]] Property<T> &property(PropertyHandle<T> ph) {

        assert_true(
            [&ph, this]() {
                return ph.indx() >= 0 and ph.indx() < static_cast<int>(properties_.size()) and
                       properties_[ph.indx()] != nullptr and
                       properties_[ph.indx()]->type_name_ == typeid(T).raw_name();
            },
            "invalid property handle");

        return *static_cast<Property<T> *>(properties_[ph.indx()].get());
    }

    template <typename T> [[nodiscard]] const Property<T> &property(PropertyHandle<T> ph) const {

        assert_true(
            [&ph, this]() {
                return ph.indx() >= 0 and ph.indx() < static_cast<int>(properties_.size()) and
                       properties_[ph.indx()] != nullptr and
                       properties_[ph.indx()]->type_name_ == typeid(T).raw_name();
            },
            "invalid property handle");

        return *static_cast<Property<T> *>(properties_[ph.indx()].get());
    }

    template <typename T>
    [[nodiscard]] PropertyHandle<T> handle(const std::string_view name) const noexcept {
        const auto it{std::find_if(properties_.begin(), properties_.end(),
                                   [&name](const std::unique_ptr<BaseProperty> &p) {
                                       if(p and p->name_ == std::string{name} and
                                          p->type_name_ == typeid(T).raw_name()) {
                                           return true;
                                       }
                                       return false;
                                   })};

        if(it != properties_.end()) {
            return PropertyHandle<T>{static_cast<int>(std::distance(properties_.begin(), it))};
        }

        return PropertyHandle<T>{};
    }

    template <typename T> void remove(PropertyHandle<T> ph) {
        assert_true(
            [&ph, this]() {
                return ph.indx() >= 0 and ph.indx() < static_cast<int>(properties_.size());
            },
            "invalid property handle");

        properties_[ph.indx()].reset();
    }

    void resize(size_t n) const {

        for(auto &&p : properties_) {
            if(p) {
                p->resize(n);
            }
        }
    }

    void clear() { properties_.clear(); }

  private:
    std::vector<std::unique_ptr<BaseProperty>> properties_;
};

template <typename T> class PointCloudBase {
  public:
    using point_type = T;
    using scalar_type = typename T::scalar_type;
    using vector_type = std::vector<point_type>;
    using span_type = std::span<point_type>;
    using const_span_type = std::span<const point_type>;

    PointCloudBase() = default;

    PointCloudBase(span_type vec)
        : data_not_owned_{vec} {}
    PointCloudBase(vector_type &&vec) noexcept
        : data_owned_{std::move(vec)} {}

    PointCloudBase(const PointCloudBase &) = default;
    PointCloudBase(PointCloudBase &&) noexcept = default;

    PointCloudBase &operator=(const PointCloudBase &) = default;
    PointCloudBase &operator=(PointCloudBase &&) noexcept = default;

    void create(size_t size) {
        data_owned_.resize(size);
        data_not_owned_ = {};
        prop_container_.clear();
    }

    void create(vector_type &&vec) noexcept {
        data_owned_ = std::move(vec);
        data_not_owned_ = {};
        prop_container_.clear();
    }

    [[nodiscard]] size_t size() const noexcept {
        return std::max(data_not_owned_.size(), data_owned_.size());
    }

    [[nodiscard]] point_type &operator[](size_t i) {
        return data_owned_.empty() ? data_not_owned_[i] : data_owned_[i];
    }

    [[nodiscard]] const point_type &operator[](size_t i) const {
        return data_owned_.empty() ? data_not_owned_[i] : data_owned_[i];
    }

    [[nodiscard]] std::span<point_type> points() noexcept {
        return data_owned_.empty() ? data_not_owned_ : data_owned_;
    }

    [[nodiscard]] std::span<const point_type> points() const noexcept {

        if(data_not_owned_.empty()) {
            return data_owned_;
        }
        return data_not_owned_;
    }

    [[nodiscard]] bool empty() const noexcept {
        return data_owned_.empty() and data_not_owned_.empty();
    }

    template <we::Prop name> PropertyHandle<detail::prop_traits_t<name>> add_property() {
        auto ph{prop_container_.add<detail::prop_traits_t<name>>(detail::prop_traits_v<name>)};
        prop_container_.resize(size());
        return ph;
    }

    template <we::Prop name>
    PropertyHandle<detail::prop_traits_t<name>>
    add_property(std::vector<detail::prop_traits_t<name>> &&data) {
        assert_true([&, this]() { return data.size() == size(); }, "wrong property size");
        return prop_container_.add<detail::prop_traits_t<name>>(detail::prop_traits_v<name>,
                                                                std::move(data));
    }

    template <we::Prop name>
    PropertyHandle<detail::prop_traits_t<name>>
    add_property(const std::span<detail::prop_traits_t<name>> data) {
        assert_true([&, this]() { return data.size() == size(); }, "wrong property size");
        auto ph{add_property<name>()};
        if(ph.is_valid()) {
            std::ranges::copy(data, property(ph).begin());
        }
        return ph;
    }

    template <typename DataType>
    void add_property(PropertyHandle<DataType> &ph, const std::string_view name) {
        ph = prop_container_.add<DataType>(name);
        prop_container_.resize(size());
    }

    template <typename DataType>
    PropertyHandle<DataType> add_property(std::vector<DataType> &&data,
                                          const std::string_view name) {
        assert_true([&, this]() { return data.size() == size(); }, "wrong property size");
        return prop_container_.add<DataType>(name, std::move(data));
    }

    template <typename DataType>
    PropertyHandle<DataType> add_property(const std::span<DataType> data,
                                          const std::string_view name) {
        assert_true([&, this]() { return data.size() == size(); }, "wrong property size");
        PropertyHandle<DataType> ph;
        add_property(ph, name);
        if(ph.is_valid()) {
            std::ranges::copy(data, property(ph).begin());
        }
        return ph;
    }

    template <typename DataType> void remove_property(PropertyHandle<DataType> ph) {
        if(ph.is_valid()) {
            prop_container_.remove(ph);
        }
        ph.reset();
    }

    template <we::Prop name> void remove_property() {
        auto ph{get_property_handle<name>()};
        if(ph.is_valid()) {
            prop_container_.remove(ph);
        }
        ph.reset();
    }

    template <typename DataType>
    [[nodiscard]] typename PropertyHandle<DataType>::span_type
    property(PropertyHandle<DataType> ph) {
        return prop_container_.property(ph).data();
    }

    template <typename DataType>
    [[nodiscard]] const typename PropertyHandle<DataType>::const_span_type
    property(PropertyHandle<DataType> ph) const {
        return prop_container_.property(ph).data();
    }

    template <typename DataType>
    [[nodiscard]] PropertyHandle<DataType>
    get_property_handle(const std::string_view name) const noexcept {
        return prop_container_.handle<DataType>(name);
    }

    template <we::Prop name>
    [[nodiscard]] PropertyHandle<detail::prop_traits_t<name>> get_property_handle() const noexcept {
        return prop_container_.handle<detail::prop_traits_t<name>>(detail::prop_traits_v<name>);
    }

    template <we::Prop name>
    [[nodiscard]] std::optional<std::span<detail::prop_traits_t<name>>> property() {
        if(auto ph{get_property_handle<name>()}; ph.is_valid()) {
            return property(ph);
        }
        return std::nullopt;
    }

    template <we::Prop name>
    [[nodiscard]] std::optional<std::span<const detail::prop_traits_t<name>>> property() const {
        if(auto ph{get_property_handle<name>()}; ph.is_valid()) {
            return property(ph);
        }
        return std::nullopt;
    }

    template <typename OtherScalar>
        requires std::is_convertible_v<scalar_type, OtherScalar> and
                 std::is_arithmetic_v<OtherScalar>
    [[nodiscard]] PointCloudBase<Matrix<OtherScalar, T::nRows, T::nCols>> cast() const {

        PointCloudBase<Matrix<OtherScalar, T::nRows, T::nCols>> res;
        res.create(size());

        auto input_pts{res.points()};
        auto out_pts{points()};

        std::transform(out_pts.begin(), out_pts.end(), input_pts.begin(),
                       [](auto &&val) { return val.template cast<OtherScalar>(); });

        res.prop_container_ = prop_container_;
        return res;
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
        writer.write_vertices(points());

        if(auto val{property<Prop::NORMALS>()}; val) {
            writer.write_normals(*val);
        }

        if(auto val{property<Prop::INTENSITY>()}; val) {
            writer.write_intensity(*val);
        }
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

        prop_container_.clear();
        data_owned_.clear();
        data_not_owned_ = {};

        if(auto vertices{reader.read_points()}; vertices) {
            data_owned_ = std::move(*vertices);
        } else {
            return false;
        }

        if(auto normals{reader.read_point_normals()}; normals) {
            add_property<Prop::NORMALS>(std::move(*normals));
        }

        if(auto intensity{reader.read_intensity()}; intensity) {
            add_property<Prop::INTENSITY>(std::move(*intensity));
        }

        return true;
    }

  protected:
    template <typename OtherT> friend class PointCloudBase;

    std::vector<T> data_owned_;
    std::span<T> data_not_owned_;
    PropertyContainer prop_container_;
};

template <typename T> class PointCloud : public PointCloudBase<T> {
  public:
    using Base = PointCloudBase<T>;
    using PointCloudBase<T>::PointCloudBase;

    template <typename OtherScalar>
        requires std::is_convertible_v<typename Base::scalar_type, OtherScalar> and
                 std::is_arithmetic_v<OtherScalar>
    [[nodiscard]] PointCloud<Matrix<OtherScalar, T::nRows, T::nCols>> cast() const {
        return PointCloud<Matrix<OtherScalar, T::nRows, T::nCols>>{
            Base::template cast<OtherScalar>()};
    }

  private:
    template <typename OtherT> friend class PointCloud;
    PointCloud(Base &&base) noexcept
        : Base{std::move(base)} {}
};

template <typename T> class StructuredPointCloud : public PointCloudBase<T> {
  public:
    using Base = PointCloudBase<T>;
    StructuredPointCloud() = default;

    explicit StructuredPointCloud(const typename Base::span_type vec, size_t width, size_t height,
                                  typename Base::point_type empty_value)
        : Base{vec}
        , width_{width}
        , height_{height}
        , empty_value_{empty_value} {}

    StructuredPointCloud(Base::vector_type &&vec, size_t width, size_t height,
                         typename Base::point_type empty_value)
        : PointCloud<T>{std::move(vec)}
        , width_{width}
        , height_{height}
        , empty_value_{empty_value} {

        assert_true([&]() { return Base::size() == width * height; }, "wrong point cloud size");
    }

    StructuredPointCloud(const StructuredPointCloud &) = default;
    StructuredPointCloud(StructuredPointCloud &&) noexcept = default;

    StructuredPointCloud &operator=(const StructuredPointCloud &) = default;
    StructuredPointCloud &operator=(StructuredPointCloud &&) noexcept = default;

    void create(size_t width, size_t height, typename Base::point_type empty_value) {
        width_ = width;
        height_ = height;
        empty_value_ = empty_value;
        Base::create(width * height);
    }

    void create(Base::vector_type &&vec, size_t width, size_t height,
                typename Base::point_type empty_value) noexcept {
        width_ = width;
        height_ = height;
        empty_value_ = empty_value;
        Base::create(std::move(vec));
    }

    PointCloud<T> pointcloud() const {
        std::vector<T> pts;
        pts.reserve(this->size());

        const auto _points{this->points()};

        for(auto &&p :
            _points | std::views::filter([this](auto &&val) { return val != empty_value_; })) {
            pts.push_back(p);
        }

        pts.shrink_to_fit();
        PointCloud<T> ret{std::move(pts)};

        auto copy_property{[this, &_points, &ret]<we::Prop name>() {
            if(auto vals{this->template property<name>()}; vals) {
                using val_t = detail::prop_traits_t<name>;
                std::vector<val_t> _vals;
                _vals.reserve(this->size());

                for(auto &&i : std::views::iota(0ull, this->size())) {

                    if(_points[i] == empty_value_) {
                        continue;
                    }

                    _vals.push_back((*vals)[i]);
                }

                _vals.shrink_to_fit();
                ret.add_property<name>(std::move(_vals));
            }
        }};

        [&copy_property]<int... I>(std::integer_sequence<int, I...>) {
            ((copy_property.template operator()<static_cast<we::Prop>(I)>()), ...);
        }(std::make_integer_sequence<int, static_cast<int>(we::Prop::LAST_PROP)>{});

        return ret;
    }

    [[nodiscard]] typename Base::point_type &operator()(size_t i, size_t j) {
        return Base::operator[](i * width_ + j);
    }

    [[nodiscard]] const typename Base::point_type &operator()(size_t i, size_t j) const {
        return Base::operator[](i * width_ + j);
    }

    [[nodiscard]] size_t width() const noexcept { return width_; }
    [[nodiscard]] size_t height() const noexcept { return height_; }
    [[nodiscard]] Base::point_type empty_value() const noexcept { return empty_value_; }
    [[nodiscard]] Base::point_type &empty_value() noexcept { return empty_value_; }

    [[nodiscard]] bool point_valid(size_t i, size_t j) const {
        return Base::points()[i * width_ + j] != empty_value_;
    }

    template <typename OtherScalar>
        requires std::is_convertible_v<typename Base::scalar_type, OtherScalar> and
                 std::is_arithmetic_v<OtherScalar>
    [[nodiscard]] StructuredPointCloud<Matrix<OtherScalar, T::nRows, T::nCols>> cast() const {
        return StructuredPointCloud<Matrix<OtherScalar, T::nRows, T::nCols>>{
            Base::template cast<OtherScalar>(), width_, height_,
            empty_value_.template cast<OtherScalar>()};
    }

  private:
    template <typename OtherT> friend class StructuredPointCloud;

    explicit StructuredPointCloud(Base &&pcd, size_t width, size_t height,
                                  typename Base::point_type empty_value)
        : Base{std::move(pcd)}
        , width_{width}
        , height_{height}
        , empty_value_{empty_value} {

        assert_true([&]() { return Base::size() == width * height; }, "wrong point cloud size");
    }

    size_t width_{0};
    size_t height_{0};

    Base::point_type empty_value_{std::numeric_limits<typename Base::scalar_type>::min()};
};

using PointCloud3f = PointCloud<we::Point3f>;
using StructuredPointCloud3f = StructuredPointCloud<we::Point3f>;

} // namespace we