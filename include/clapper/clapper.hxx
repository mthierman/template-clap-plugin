#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

#include <array>

namespace clapper {
using PluginHelper = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Ignore,
                                           clap::helpers::CheckingLevel::None>;
} // namespace clapper
