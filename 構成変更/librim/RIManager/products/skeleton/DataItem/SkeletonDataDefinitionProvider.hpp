class SkeletonDataDefinitionProvider
    : public IDataDefinitionProvider
{
public:

    const std::vector<RIMDataDefinition>&
    GetDefinitions() const override;
};