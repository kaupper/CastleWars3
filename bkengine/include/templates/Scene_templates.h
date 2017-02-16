#ifndef SCENE_TEMPLATES_H
#define SCENE_TEMPLATES_H

template <typename T> T &Scene::addElement(const T &element)
{
    return addElement<T>(std::make_shared<T>(std::move((T &) element)));
}

template <typename T> T &Scene::addElement(const std::shared_ptr<T> &element)
{
    elements.push_back(element);
    return *((T *) elements.back().get());
}

template <typename T, typename... Params> T &Scene::addElement(Params... params)
{
    return addElement<T>(std::make_shared<T>(params...));
}


template <typename T> T &Scene::getElement(const std::string &name)
{
    for (auto &element : elements) {
        if (element->getDescription() == name) {
            return *((T *) element.get());
        }
    }

    Logger::LogCritical("Scene::getElement(const std::string &=" + name +
                        "): Element not found");
    throw "Element not found";
}

template <typename T, typename... Params> T &Scene::getElement(
    unsigned int index)
{
    if (index < elements.size()) {
        return *((T *) elements[index].get());
    }

    Logger::LogCritical("Scene::getElement(int=" + std::to_string(
                            index) + "): Element not found");
    throw "Element not found";
}

#endif
